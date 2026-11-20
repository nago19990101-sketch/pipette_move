# 通常視野クリック → インジェクションピペット移動 機能 実装計画

（Part A が終わったら `feature/click-to-move` ブランチで実施します）

## Context（背景と目的）
`garubano2/main_oshima.cpp` のシステムでは、インジェクションピペット（man1=右上RU / man3=右下RD、Micro Support MCUSB4sd ボード）を Phantom や Xbox で手動操作しています。
目標は、**通常視野ウィンドウ `circle_img` 上で点をクリックし、確認キーを押すと、操作中のインジェクションピペットの先端がその点へ XY 移動する**機能を追加することです（Z は現在の高さのまま固定）。

ユーザーの指定:
- クリックで目標を指定し、確認キーで移動を開始する
- Z は固定し、XY のみ動かす
- 座標変換は既存のキャリブレーション（`rvec1/tvec1`, `rvec3/tvec3`, `K`, 先端補正 `man*_dx/dy`）を使う
- 対象は `move_mode` で選ばれている操作中のインジェクションピペット
- クリックするのは通常視野 `circle_img` だけ

### 既存コードについて分かったこと
- 正変換はすでにあります。`projectPoints(man*_pos3D, rvec*, tvec*, K, distortion, man*_pos2D)` に `man*_dx/dy` を足した値が `man*_cpos` で、通常視野では main_oshima.cpp:9141-9172 で計算しています。
  - `man*_pos3D` は `(xw, -yw, zw)` の形で、**y の符号が反転**しています。
- 逆変換は保持ピペット用の `camera_to_world1/2`（:2211-2234）しかありません。しかも2つの問題があります。
  - y に `cx` と `_dx` を使っている（本来は `cy` と `_dy`）
  - `R(p−t)` で計算している（本来は `Rᵀ(p−t)`）
  
  → **インジェクション用には、正しい逆変換を新しく書きます。**
- 通常視野では man3 の `projectPoints` が抜けています（拡張視野の :8417-8430 にしかありません）。
- `circle_img` のマウス処理（:9088-9089）は、ループのたびにローカル変数 `mouseParam mouse` を登録し直していて、壊れています。
- 移動関数 `Movepos(h, speed, pos)`（microsupportcommand.cpp:164）は**完了を待ちません**。
- 他のスレッドが同じデバイスを操作しているので、移動中はそれを止める必要があります。
  - Xbox モード（`use==2`）では、mythread7 が毎ループ `Go(hDevice_move_r, speed_r)` を呼びます（:5482）。速度0のときは `DriveStop` になるので、そのままだと Movepos がすぐ止められてしまいます。
  - Phantom モードでは、mythread9 が `Movepos(hDevice_move_r, …)` を出し続けます（:6186, :6224）。
- 既存の安全なパターンは A キーのシーケンス（:4810-4864）です。手順は次のとおりで、これを踏襲します。
  1. `move_flag=0; move_mode=0`
  2. `Movepos`
  3. `man*_xw/yw/zw` を更新し、`man*_xw0 = xw - pPosition*Kp_pha` で Phantom のオフセットを再設定
  4. フラグを元に戻す
- キー割り当ての空き状況（GetAsyncKeyState の使用箇所を調べた結果）: **Enter (VK_RETURN)・T (0x54)・X (0x58) は未使用**です。

## 操作方法（新機能）
| 操作 | 動作 |
|---|---|
| **T を押しながら `circle_img` を左クリック** | 目標点を設定し、画面に表示する。現在の先端から目標まで線を引き、移動量（µm）を表示する。コンソールには目標のマニピュレータ座標を出力する |
| **Enter** | 目標点へ移動を開始する（押した瞬間だけ反応するエッジ検出） |
| **X** | 目標をクリアする。移動中なら即停止する（`DriveStop`） |

## 実装内容

### 1. 新規ファイル `garubano2/click_move.h` / `click_move.cpp`（純粋な座標計算）
- `bool pixelToWorldFixedZ(double u, double v, double zw, const cv::Mat& rvec, const cv::Mat& tvec, const cv::Mat& K, double tip_dx, double tip_dy, double& xw, double& yw);`
  1. 正規化光線 `d = ((u - tip_dx - K(0,2))/K(0,0), (v - tip_dy - K(1,2))/K(1,1), 1)` を作る
  2. `R = Rodrigues(rvec)` とし、`a = Rᵀd`、`b = Rᵀt` を計算する
  3. ワールド座標の z が `zw` になる条件から `λ = (zw + b.z) / a.z` を求め、`P = λa − b` とする
  4. `xw = P.x`、`yw = -P.y` を返す（`man*_pos3D` の y 反転を元に戻す）
  5. `a.z` がほぼ0なら false を返す
- `cx/cy` のグローバル変数は使わず、`K` の要素を直接使います（正変換と完全に一致させるため）。
- 検算用に `reprojectError(...)` も用意します。結果を `projectPoints` で再投影し、クリック点とのずれ（px）を返します。
- 文字コードは main_oshima.cpp に合わせて UTF-8（BOM付き）にします。
- `garubano2.vcxproj` に `<ClCompile Include="click_move.cpp" />` と `<ClInclude Include="click_move.h" />` を追加します。

### 2. `main_oshima.cpp` への組み込み
**a. グローバル変数**（:530 付近のフラグ群の近く）
```cpp
std::atomic<bool> g_click_target_set{false}, g_click_move_busy{false}, g_click_move_abort{false};
double g_click_target_px[2];      // circle_img 上のクリック座標
mouseParam g_circleMouse{0,0,-1,0};
const double CLICK_MOVE_SPEED = 300.0;     // µm/s（既定の s=1000 より遅め）
const double CLICK_MOVE_MAX_DIST = 1000.0; // µm、これを超える移動は拒否
```

**b. `circle_img` のマウスコールバックを作り直す**（:1999 の `CallBackFunc` の代わり）
- `CircleImgMouseCallback`: `g_circleMouse` に記録し、`EVENT_LBUTTONDOWN` で T キーが押されていれば `g_click_target_px` を設定して `g_click_target_set=true` にする。
- :9088-9089 を書き換え、`setMouseCallback("circle_img", CircleImgMouseCallback)` を**最初の1回だけ**登録します（static bool）。
- 既存の「1〜4 キー＋クリックで先端補正」（:9090-9111）は `g_circleMouse` を参照するように変え、判定が終わったら `g_circleMouse.event = -1` に戻します（既存の不具合も一緒に直ります）。

**c. 通常視野で man3 の投影を追加**
- :9141-9172 の man1 ブロックのすぐ後に、man3 版を追加します（`rvec3/tvec3/rmat3/man3_dx/dy`）。中身は拡張視野 :8417-8430 と同じです。
- 描画（:9265）はすでにあるので、通常視野でも man3 の先端マーカーが正しく出るようになります。

**d. 目標の表示と Enter/X の判定**（通常視野ループ内、`imshow("circle_img")` :9515 の直前）
- `g_click_target_set` が true のとき、目標に十字マーカーを描き、操作中ピペットの `man*_cpos` から目標まで線を引きます。
- Enter の立ち上がりを検出したら、条件をすべて確認してから `std::thread(clickMoveExecute).detach()` を起動します。表示ループは止めません。条件は次のとおりです。
  - `!g_click_move_busy`
  - 目標が設定されている
  - `move_mode` が 1 か 2
  - 対応する `man*_flag == point_number + 2`（rmat が計算済み）
- X が押されたら、目標をクリアし、`g_click_move_abort=true` にします。

**e. 移動処理 `clickMoveExecute()`**（新しい関数、`moveToPosition` :6893 の待ち方を参考にする）
1. `move_mode` から対象を決めます（2 → `hDevice_RU`/man1/`rvec1,tvec1,man1_dx,dy`、1 → `hDevice_RD`/man3/…）。
2. `g_click_move_busy=true` にし、元の `move_mode`/`move_flag` を保存してから `move_flag=0; move_mode=0;`（Phantom 出力が止まる）。
3. `Getposition(dev, x0,y0,z0)` で現在位置を取得します。`pixelToWorldFixedZ(target_px, z0, …)` で `(xt,yt)` を求めます。
   - 移動量が `CLICK_MOVE_MAX_DIST` を超えるときや計算に失敗したときは、移動せずにフラグを戻して終了します。
   - コンソールに現在位置、目標位置、移動量、再投影誤差を出力します。
4. `Movepos(dev, {SPEED,SPEED,SPEED}, {xt, yt, z0})` を呼びます。Z は移動量0なので、その軸は停止のままです。
5. 5 ms ごとに `Getposition` で確認し、次のどれかで終了します。
   - 誤差が 1 µm 以下になった
   - タイムアウト（`距離/SPEED + 2 s`）
   - `g_click_move_abort` または X キー → `DriveStop(dev)`
6. 最終位置で `man*_xw/yw/zw` を更新します。`man*_xw0 = xw - pPosition[0]*Kp_pha`、`yw0 = yw - (-pPosition[2]*Kp_pha)`、`zw0 = zw - pPosition[1]*Kp_pha_z` とし、Phantom に戻ったときに跳ね戻らないようにします。
7. `move_mode`/`move_flag` を元に戻し、`g_click_move_busy=false`、`g_click_target_set=false` にします。

**f. 他スレッドとの競合を防ぐ**（それぞれ1行の条件追加）
- mythread7 :5482 の `Go(hDevice_move_r, speed_r)` は、`!g_click_move_busy` のときだけ呼ぶようにします（Xbox 中に Movepos が止められないように）。
- mythread9 の :6186 / :6224 は `move_mode=0` で止まりますが、念のため `!g_click_move_busy` の条件も加えます。

## 変更するファイル
- `garubano2/click_move.h`, `garubano2/click_move.cpp`（新規）
- `garubano2/main_oshima.cpp`（上記 a〜f）
- `garubano2/garubano2.vcxproj`（ビルド対象に追加）

## 動作確認
1. **ビルド**: VS（garubano2.sln、x64 Release）でビルドが通ることを確認します。MSBuild が使えればこちらでも実行します。
2. **計算の検算（ハード不要）**: T＋クリックしたときにコンソールへ出る再投影誤差が 0.1 px 未満であることを確認します（逆変換と正変換が一致している証拠）。
3. **実機（ユーザーが実施）**
   1. 起動して通常視野（S+2）にし、緑の先端マーカーが実際の先端に重なっていることを確認します。ずれていれば、既存の「1/3＋クリック」で補正します。
   2. 先端の近く（50〜100 µm 程度）を T＋クリックし、表示とコンソールの移動量を確認してから Enter を押します。先端がクリック点に来ること、Z（焦点）が変わらないことを確認します。
   3. 移動中に X を押して止まることを確認します。
   4. 移動後に Phantom / Xbox の操作へ戻り、ピペットが跳ねずにその位置から続けて操作できることを確認します。
   5. A キーで man1/man3 を切り替え、もう一方のピペットでも同じように動くことを確認します。
4. 移動量の上限（1000 µm）を超える点をクリックした場合に、移動が拒否されることを確認します。

## 注意点
- 精度は既存のキャリブレーション（ハードコードされている `rvec/tvec`）と先端補正 `man*_dx/dy` に依存します。緑マーカーが先端と合っていれば、クリック点にも合います。
- Enter と X は `GetAsyncKeyState` で読むので、コンソールなど他のウィンドウで押しても反応します（既存のキーと同じ仕様）。
