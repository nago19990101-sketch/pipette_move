# INFINICAM 視野拡張システム（ミラー変更版）

INFINICAM（Photron）の高速カメラとガルバノミラーを使った視野拡張顕微鏡システムです。Micro Support のマニピュレータ（MCUSB4sd）を Phantom や Xbox で操作し、Oriental Motor のインジェクタ（Modbus）を制御します。

## 構成
| パス | 内容 |
|---|---|
| `garubano2.sln` | Visual Studio ソリューション（v142 / x64） |
| `garubano2/main_oshima.cpp` | ビルド対象のメインプログラム（UTF-8 BOM） |
| `garubano2/main.cpp` | 旧メイン（ビルド対象外。参照用） |
| `garubano2/microsupportcommand.cpp` | マニピュレータ移動関数（Movepos / Go / Getposition など） |
| `garubano2/get_create_image.cpp` | 視野拡張画像の生成 |
| `libmodbus/` | libmodbus と `ModbusCVD`（インジェクタ制御） |
| `garubano2/Include_*`, `include_*` | ベンダー SDK のヘッダ |
| `docs/` | 計画書など |

ソースの文字コードは混在しています。`main_oshima.cpp` は UTF-8、それ以外の多くは Shift-JIS（CP932）です。そのため GitHub の画面では Shift-JIS のファイルの日本語が文字化けして見えますが、Visual Studio 上では問題ありません。

## ビルド
1. `garubano2.sln` を Visual Studio（v142 ツールセット）で開き、**Release / x64** を選びます。
2. NuGet パッケージ（`Microsoft.ML.OnnxRuntime 1.20.0`）は `packages.config` から復元します。
3. 次の外部 SDK が、プロジェクト設定に書かれたパスにインストールされている必要があります。
   - OpenCV 4.8.0: `C:\opencv\build`
   - OpenHaptics 3.5.0: `C:\OpenHaptics\Developer\3.5.0`
   - LibTorch: `C:\libtorch`
   - Python 3.9
   - Photron PUCLib
4. `libmodbus` のパスは、`garubano2.vcxproj` に絶対パスで書かれています。フォルダの場所を変えたときは修正してください。

## リポジトリに含めていないファイル
次のファイルは容量が大きいため、Git 管理から外しています（`.gitignore` を参照）。別の PC で動かすときは、元の PC から `garubano2/` などへ手動でコピーしてください。

- **DLL 一式**（`garubano2/*.dll`, `garubano2/bin/*.dll`, `libmodbus/modbus.dll`）
  - ボード: Fbi*、IFUcnt、Hpmcstd
  - カメラ: PUCLIB、PDCLIB、xiapi
  - OpenCV、OpenHaptics（hd/hl）、glut
  - torch / CUDA / cuDNN 系
  - onnxruntime
- **モデル**
  - `garubano2/streaming_2stream_model_cpp.pt`
  - `garubano2/streaming_model_cpp.pt`
  - `garubano2/train_opset17_117_4to103_add_size80_4layer_005.onnx`
- **データ**
  - CSV ログ（`*.csv`）、動画（`*.mp4`）、画像（`*.png`, `*.jpg`）
  - `holdingpipette_img/`、`injectionpipette_img/`、`logging/`
- **ビルド成果物**: `Debug/`、`Release/`、`x64/`、`garubano2/bin/`、`.vs/`
- **旧バックアップ**: `cpp_backup/`（今後は Git の履歴で管理します）
