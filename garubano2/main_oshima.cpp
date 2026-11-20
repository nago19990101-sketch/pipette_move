#define _CRT_SECURE_NO_DEPRECATE

//大島追加
#define NOMINMAX
#include <winsock2.h>


#include <torch/torch.h>
#include <torch/script.h>
//#include <torch/cuda.h>

#include <windows.h>
#include <stdio.h>
#include <string>
#include "FbiDa.h"
#include "math.h"
#include "FbiAd.h"
#include "fbipenc.h"
#include <stdlib.h>
#include <codecvt>
#include "Function.h"									//青山便利グッズ
#include <sys/stat.h>
#include <direct.h>//ここから下ほぼ使ってない
#include <fstream>
#include <sstream>
#include <iomanip>
#include "timer.h"
#include <random>
std::random_device rd;
std::mt19937 gen(rd());

//opencv247
//#include "opencv/cv.h" 
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/dnn_superres.hpp"
//#include <opencv_lib.hpp>
#include "opencv2/dnn.hpp"

//xbox
#include <Xinput.h>
#include "microsupportcommand.hpp"
#include "click_move.h"	//クリック移動（通常視野クリック→インジェクションピペット移動）
#pragma comment(lib,"winmm.lib")			
#pragma comment(lib, "xinput.lib")

std::vector<double> s{ 1000,1000,1000 };//ピペット操作速度

HANDLE			hDevice_RU;//右側のマニピュレータにアクセスするハンドル
HANDLE			hDevice_RD;//右側のマニピュレータにアクセスするハンドル
HANDLE			hDevice_LU;//右側のマニピュレータにアクセスするハンドル
HANDLE			hDevice_LD;//左側のマニピュレータにアクセスするハンドル
HANDLE			hDevice_move_r;//右スティックで動くマニピュレータのハンドル
HANDLE			hDevice_move_l;//左スティックで動くマニピュレータのハンドル
XINPUT_STATE xbox;

double pos_x, pos_y, pos_z;
const CHAR* szBoadName = "MCUSB4sd";

//Photron
#include "PhotronVideoCapture.h"
#include "PUCLib_Wrapper.h"
#include "PUCLIB.h"
photron::VideoCapture cap;

//シリアル通信
#include "calculatecommand.h"
#include "crc16ibm.h"

//ミラー角度導出、画像保存、作成
#include "get_create_image.h"
#include "output_message.h"
#include "class_test.h"
test::MyClass ClTest;

#pragma comment(lib, "PDCLIB.lib")
#pragma comment(lib, "FbiDa.lib")
#pragma comment(lib, "FbiAd.lib")
#pragma comment(lib,"FbiPenc.lib")						//カウンタボードのライブラリー
#pragma comment(lib,"winmm.lib")	

//phantom
#include <HD/hd.h>
#include <HL/hl.h>
#include <HL/hlCompilerConfig.h>
#include <HL/hlDefines.h>
#include <HD/hdDevice.h>
#include <HL/hlExport.h>
#include <HD/hdScheduler.h>
#include <HL/hlTypes.h>
#include <GL/glut.h>
#include <GL/glui.h>
#include "phantom_command2.h"
int use = 2; //xboxとphantomのきりかえ 2でxbox、1でphantom

//モータ関連
#include "dhdc.h"
#include "drdc.h"
#include <time.h>
#include "PDCLIB.h"
#include "PDCSUBSTR.h"
#include "PDCFUNCHide.h"
#include "IFHSCNT.h"  
#include "IFHSCNT.H"
#include "IFUCNT.H"

//onnx
#include "onnxruntime_cxx_api.h"
#include <omp.h>
//
//#include <torch/torch.h>
////#include <torch/script.h>
////#include <torch/cuda.h>

//Phantom,Omega用
double Kp_pha = 9.0;//Phantomの座標からman1の座標に変換する際の比例ゲイン
double Kd_pha = 0.0;//Phantomの座標からman1の座標に変換する際の微分ゲイン
double Kp_pha_z = 0.0;
hduVector3Dd pPosition;//取得したPhantomの座標
hduVector3Dd pPosition_past;//１ループ前のPhantomの座標
hduVector3Dd pVelocity;

double pPosition2[3] = { 0,0,0 }; //フィルタ後のphantom座標
//hduVector3Dd vel_pha;//Phantomの移動速度
std::vector<double> vel_phantom{ 100, 100, 100 };
std::vector<double> move_man1_pos;
std::vector<double> move_man2_pos;
std::vector<double> move_man3_pos;
std::vector<double> move_man4_pos;
double static_z1;
double static_z3;
double pvel_x, pvel_y, pvel_z;
//double ppPosition[3] = { 0, 0, 0 };//Phantomのひとつ前の座標
double man1_xw0, man1_yw0, man1_zw0;//世界座標系でのman1の初期座標
double man2_xw0, man2_yw0, man2_zw0;//世界座標系でのman2の初期座標
double man1_xw, man1_yw, man1_zw;//世界座標系でのman1の取得座標
double man2_xw, man2_yw, man2_zw;//世界座標系でのman2の取得座標
double man3_xw0, man3_yw0, man3_zw0;//世界座標系でのman3の初期座標
double man4_xw0, man4_yw0, man4_zw0;//世界座標系でのman4の初期座標
double man3_xw, man3_yw, man3_zw;//世界座標系でのman3の取得座標
double man4_xw, man4_yw, man4_zw;//世界座標系でのman4の取得座標
double set_man1_xw, set_man1_yw, set_man1_zw; //座標設定アルゴリズムで使う
double set_man2_xw, set_man2_yw, set_man2_zw; //座標設定アルゴリズムで使う
double set_man3_xw, set_man3_yw, set_man3_zw; //座標設定アルゴリズムで使う
double set_man4_xw, set_man4_yw, set_man4_zw; //座標設定アルゴリズムで使う

double man13_store[3] = { 3350, -2500, -2200 };
double man1_auto_dz = 0;
double man3_auto_dz = 0;

double pre_man24_cpos_ex[3];
double pre_man24_pos[3];
//それぞれのピペットのセットポジションおよびホームポジション
//double man1_set[3] = {4250,-1280,-2400};
//double man2_set[3] = {2100,-1280,-2400};
//double man3_set[3] = {4250,-2630,-2400};
//double man4_set[3] = {2100,-2630,-2400};
//double man1_home[3] = {};
//double man2_home[3] = {};
//double man3_home[3] = {};
//double man4_home[3] = {};
//

//スリットの位置検出用
int C_radius;
int C_radius_ex;
int C_radius_c;
int C_radius_c2;
double radius_ratio = 1.1;
double C_center_x, C_center_y;
double slit_x, slit_y;
double C_center_xc, C_center_yc;
double slit_xc, slit_yc;
double C_center_xc2, C_center_yc2;

double past_man1_xw, past_man1_yw, past_man1_zw;//世界座標系でのman1の1つ前の取得座標
double past_man3_xw, past_man3_yw, past_man3_zw;//世界座標系でのman1の1つ前の取得座標
int man1_MorR = 1;
//粘性計算用
double gain;
double b_framex, b_framey;//視野拡張時の中心視野のサイズ(s_dwdによって変化)
double b_width; //ウィンドウ上1mmの長さ
int phantom_flag = 0;//キープ状態 1で視野拡張　2で通常視野
//操作中のインジェクションピペット座標(カメラ座標系)
double point_man_x;
double point_man_y;
double point_man_x_s;
double point_man_y_s;
int center_flag = 0;
int m_flag = 0; //操作感覚計測用フラグ//0で通常　//1で計測
double man1_mpoint3D[3]; //操作感覚計測用座標
double man1_mpoint2D[3];
double man1_mpoint2D_ex[3];
double man1_mpoint2D_c[3];
double man1_cposx_s;
double man1_cposy_s;
double man1_cpos_c[3];
//double man2_cpos_c[3];
double man24_cpos_c[3]; //1102small画像でのホールディングピペットのカメラ座標
Mat man1_mpos2D(1, 3, CV_64FC1);
Mat man1_mpos3D(1, 3, CV_64FC1);
double Kc = 0;
int get_image_flag = 0;

int move_mode = 1; //1：下が動く 2：上が動く 0:どっちも動かない
int move_flag = 3; //0：動かない 1：1軸 2：2軸(平面)動く 3：3軸
int move_flag2; //move_flag保存用
int phase_flag = 0; //0: なんもなし、1: 体細胞保持、2: 卵子の移動&回転、3:徐格、4:体細胞挿入
int height_flag = 0; //0：なんもなし 1：回転時の高さ 2：除核時の高さ
int phase_2_flag = 0;

double Pcenter_dx_ex;
double Pcenter_dy_ex;
double Pcenter_dx;
double Pcenter_dy;

double diff_centerx_s;
double diff_centery_s;

#pragma comment(lib, "xiapi64.lib")
#pragma comment(lib, "PDCLIB.lib")
#define snprintf _snprintf

using namespace std;
using namespace cv;
using namespace mytimer;

#include "stdafx.h"

#ifdef WIN32
//#include "xiApi.h"       // Windows
#else
#include <m3api/xiApi.h> // Linux, OSX
#endif
#include <memory.h>

#define HandleResult(res,place) if (res!=XI_OK) {printf("Error after %s (%d)\n",place,res);goto finish;}

// image buffer
//XI_IMG image;
Mat image_phot;
Mat img_phot;
Mat img_resize;
Mat circle_img;
Mat t_frame;
Mat small_img;
Mat Labelmg_slit;

cv::Mat bg;

#define IMG_WIDTH2    	1246//648 //画像の大きさ（横）
#define IMG_HEIGHT2		1008//488	//画像の大きさ（縦）1008から800へ変更２０２３/１１/３０

//グローバル変数
double Time[nstoreimage];
double Time2[nstoreimage];
double Time3[nstoreimage];
double Time4[nstoreimage];
double Time5[nstoreimage];
int TaskNum = 0;
int count1;												// 割り込みで使用
double w = 1400;                                    //角速度[rad/s]

//画像保存用
int p_i = 0;
int p2_i = 0;

UINT32* max_width;
UINT32* max_height;

//Da
HANDLE			ghDeviceHandle_da1;	//デバイスハンドル(DA用1)
HANDLE			ghDeviceHandle_da2;	//デバイスハンドル(DA用2)
DASMPLCHREQ		gstrDaSmplChReq[8];	//チャンネルレンジ設定構造体
WORD pData[2];	//DAボードのデータ

//Ad
HANDLE	hDeviceHandle_ad;		// デバイスハンドル
ADBOARDSPEC 	BoardSpec;
ADSMPLREQ		AdSmplConfig;
unsigned char	bSmplData[2];
unsigned short	wSmplData[2];
unsigned long	dwSmplData[2];
WORD data_ad[2];

int ic = 0;
char lpszName[64];			   //DA,ADデバイスオープンのときに使う
/*----Ad_variable----*/
double c1, c2;	//ADで取得したミラーの角度
int		nRet;
ADSMPLCHREQ	AdSmplChReq[2];
ADBMSMPLREQ SmplConfig;     // サンプリング条件設定構造体

//初期値
//double idoux = 0.0;	//ミラーの角度(pan軸)
//double idouy = 0.0;	//ミラーの角度(tilt軸)

//double dix = 0.0;	//駆動させる角度(pan軸)
//double diy = 0.0;	//駆動させる角度(tilt軸)

double anglex = 4.0;	//画像の名前(pan軸)
double angley = 0.0;	//画像の名前(tilt軸)
int return_cnt2 = 1;  //画像の名前（往復回数）

//輝度値，エッジ強度導出用
double average_brightness; //明るさ平均
double average_edge;		//エッジ強度平均
double average_edge2;		//正規化エッジ強度平均
UINT8 gray[IMG_HEIGHT * IMG_WIDTH];	//オフセット画像の輝度値を格納
int gray2[IMG_HEIGHT * IMG_WIDTH];//入力画像の輝度値を格納
double f[IMG_HEIGHT * IMG_WIDTH];	//エッジ強度
double contrast_col[1000];
double contrast_row[1000];
//int max_brightness_row[1000];
//int min_brightness_row[1000];
//int max_brightness_col[1000];
//int min_brightness_col[1000];
LARGE_INTEGER freq;
LARGE_INTEGER start_cnt, end_cnt;

//ETL（焦点可変レンズ）制御用変数
double ETL_Temp; //焦点可変レンズの温度
double f_Ob = 20;     //対物レンズの焦点距離 //M=20のとき１０，M=10の時２０
double etl; //ETL出力Vol
double etl_current; //ETL出力Vol
double n = 1.3;       //屈折率q(1.3)
double s_dWD = 0.0; //-0.2 //-0.63(細胞) //0.35(20240909) //0.5(20250911)
double s_dWD2 = s_dWD;
double dWD = s_dWD;       //ワーキングディスタンス変位（WDが最小値のときからの変位
double s_ddWD = 0.00;
double ddWD = 0.003;
double a_d, b_d, c_d, d;  //ETL-OL（オフセットレンズ f=-150）間距離d,導出のための二次方程式の係数a,b,c
double fETL;              //ETLの焦点距離
double fETOL;             //ETL+OLの焦点距離
double fObETOL = f_Ob;      //対物レンズ、焦点可変レンズの複合焦点距離
double fTL = 200;     //結像レンズの焦点距離
double dpt;             //ETL+OLの焦点距離の逆数
double current;	//ETLの出力値
double current_max = 292.84;
double current_min = 0;
double fOL = -150;             //OLの焦点距離
double lOL_ObL = 65.051 + 0;	//OLから対物レンズの後側焦点位置までの距離
double d_max = 7.938;
int X = 2;  //視野拡張率
int X2 = 2;  //視野拡張率
double z_in = 0;
double WD = 33; //M=10ワーキングディスタンス33、M=20ワーキングディスタンス29.2
double l1 = 9.5;//レンズからミラー距離 mm 11.2 //8.3 顕微鏡によって変わる　視野拡張の画像同士のつなぎに影響
double l2 = 9.5;//ミラーからミラーまでの距離 mm
double l3 = WD + s_dWD - l1 - l2; //ミラーから平面までの距離 mm
double D_ob = 1;//観察対象の厚さμm
double DoF_f = 15;//Dopt of Focus front
double DoF_b = -15;//Dopt of Focus back
double magnification = 10; //倍率
double resolution = 1.2;  //画像作成時の分解能
//double pixelpitch = 7.4;//ピクセルピッチ7.4μm
double pixelpitch = 10;	// phot
double** plist;            /* 入力角度へのポインタ */
double* plist_angle[1000];
double** pangle;            /* 出力角度へのポインタ */
unsigned char** pimage;            /* 画像情報へのポインタ */
double** ppointedangle;            /* 出力角度へのポインタ */
unsigned char* contrast_pimage = (unsigned char*)malloc(IMG_HEIGHT * IMG_WIDTH * sizeof(unsigned char));
unsigned char** ppointedimage;            /* 画像情報へのポインタ */
int ndata[30];          //視野拡張数Nにたいする入力画像の数=ndata[N]
int ndata2[30];          //視野拡張数Nにたいする入力画像の数=ndata[N]
int pointed_count = 0;
int t_pointed_count = 0;
int pointed_pixelx[max_pointed_count];
int pointed_pixely[max_pointed_count];
double pointed_mirrorx[max_pointed_count];
double pointed_mirrory[max_pointed_count];
double pointed_WD[max_pointed_count];
int frame_width;
int frame_height;
double MAG_IMG = 1.0;
double s_MAG_ExpandedIMG = 1 + X * 2;
double MAG_ExpandedIMG = s_MAG_ExpandedIMG;
double min_MAG_ExpandedIMG = 0.3;
double dMAG = 0.001;
double D = 10;   //焦点可変レンズの直径 mm
double a_f = -D * D / 8 / (n - 1);
double b_f = d_max;
double dl;
int frame_width2;
int frame_height2;

double mag_pointed_frame = 0.8;//0.5// 1.8//0.8; //呈示する選択画像のサイズ調整

double xangle;
double yangle;

double s_mirror_x = -0.01;
double s_mirror_y = -0.4;
double mirror_x = s_mirror_x;
double mirror_y = s_mirror_y;

int mouse_posx = 0;
int mouse_posy = 0;
int mouse_dirx = 0;
int mouse_diry = 0;
int center_framex = frame_width / 2;
int center_framey = frame_height / 2;

//タイマー用
LARGE_INTEGER qpf;
LARGE_INTEGER qpc1;
LARGE_INTEGER qpc2;
LARGE_INTEGER qpc_cut1;
LARGE_INTEGER qpc_cut2;


//三次元観察用
#define LIMIT	15	//円の検出数の上限
int Threshold = 70;//75でギリギリ、80だとピペットのインジェクションピペットの先端映る//円検出用の閾値/85 //70
int Threshold2 = 130;//円検出用の閾値/85 //70
int NOC_auto;
int dia_max = 200;//検出する円の最大直径 pixel、M=10のとき200、M=20のとき400
int dia_min = 80;//検出する円の最小直径 pixel、M=10のとき100、M=20のとき200
int dia_max_ex = 100;
int dia_min_ex = 20;//拡張画像の円検出の径
int dia_min_check = 5;
double delta_s_dWD = 0.02;//奥行き方向の撮影間隔 mm
double center_s_dWD[LIMIT];//合焦時のdWD（フィルタ前）
double center_s_dWD1[LIMIT];//合焦時のdWD（フィルタ前）1周期前
double center_s_dWD2[LIMIT];//合焦時のdWD（フィルタ後）
double ccd_width = 4.8;//カメラのセンサーサイズ（横）mm
double z1;
double z2;
double z3;
double f_sum[3][LIMIT] = { 0 };//合計エッジ強度
double f_sum1[LIMIT];
double f_sum2[LIMIT];
double f_sum3[LIMIT];
double s_dWD_0 = s_dWD;
int flag = 0;
bool search_flag = false;//ガウス分布探索のフラグ
double center_z2[LIMIT];//球の中心のz座標 um
double radius2[LIMIT];//球の半径 um
double temp;
double mag_small_img = 3.0; //岐阜大サイズまでトリミングして調整

double center_x[LIMIT];//球の中心のx座標 pixel
double center_y[LIMIT];//球の中心のy座標 pixel
double center_z[LIMIT];//球の中心のz座標 ㎛

double radius[LIMIT];//球の半径 pixel
double s_time = 0.04;//サンプリングタイム s
double omega = 30.0;//折点周波数　Hz

int polar_body = false;
int polar_x;
int polar_y;

int c_detection = 0;//円検出
int left_circle_number;
int NOC = 0;
int left_circle_flag;
int left_circle_x = 0;
int left_circle_y = 0;

int center_handle = 0; //搬送時 中央に来ているホールディングのハンドル

//マニピュレータキャリブレーション用
int num_x = 9;	//キャリブレーションパターンのx方向の数18//photron x10 自分で画像サイズにあわせて調整
int num_y = 9;	//キャリブレーションパターンのy方向の数12//photron x10
int num_z = 4;	//キャリブレーションパターンのz方向の数3
int dx = 130;	//キャリブレーションパターンのx方向の幅（μm）//photron
int dy = 110;	//キャリブレーションパターンのy方向の幅（μm）//photron120
int dz = 100;	//キャリブレーションパターンのz方向の幅（μm）
int point_number = num_x * num_y * num_z;//マニピュレータのキャリブレーションに用いる点の数
int man1_bool = 0;
int man2_bool = 0;
double man1_cpos[3];
double man2_cpos[3];
double man3_cpos[3];
double man4_cpos[3];
double man1_cpos_ex[3];
double man2_cpos_ex[3];
double man3_cpos_ex[3];
double man4_cpos_ex[3];
double man24_cpos_ex[3];
Mat objectPoints1(point_number, 3, CV_64FC1);
Mat imagePoints1(point_number, 2, CV_64FC1);
Mat objectPoints2(point_number, 3, CV_64FC1);
Mat imagePoints2(point_number, 2, CV_64FC1);
Mat objectPoints3(point_number, 3, CV_64FC1);
Mat imagePoints3(point_number, 2, CV_64FC1);
Mat objectPoints4(point_number, 3, CV_64FC1);
Mat imagePoints4(point_number, 2, CV_64FC1);
Mat K;
Mat distortion = (Mat_<double>(4, 1) << 0, 0, 0, 0);
double f_k;//pnp問題用の焦点距離

//行列内の値はキャリブレーションで得た値を用いる
//Mat rvec1 = (Mat_<double>(3, 1) << -0.0904357, 0.127686, -3.13095);
//Mat tvec1 = (Mat_<double>(3, 1) << 2621.33, 2327.07, -216215);//座標変換用並進ベクトル //20241125
Mat rvec1 = (Mat_<double>(3, 1) << -0.0256016, 0.0209369, -3.1256);
Mat tvec1 = (Mat_<double>(3, 1) << 2475.72, 2508.91, -220910);//座標変換用並進ベクトル //20241125
Mat rmat1;//座標変換用回転行列
Mat rvec2 = (Mat_<double>(3, 1) << 0.0257988, 0.0294141, 3.11892);
Mat tvec2 = (Mat_<double>(3, 1) << 2589.37, 2529.09, -219297);//座標変換用並進ベクトル
Mat rmat2;//座標変換用回転行列
Mat rvec3 = (Mat_<double>(3, 1) << 0.0316376, -0.00889257, 3.12064);
Mat tvec3 = (Mat_<double>(3, 1) << 2597.95, 2435.54, -220409);//座標変換用並進ベクトル
Mat rmat3;//座標変換用回転行列
//Mat rvec4 = (Mat_<double>(3, 1) << 0.11558, 0.0211806, 3.12489);
//Mat tvec4 = (Mat_<double>(3, 1) << 2745.15, 2285.34, -217203);//座標変換用並進ベクトル
//Mat rvec4 = (Mat_<double>(3, 1) << 0.177117, -0.0770918, 3.12332);
//Mat tvec4 = (Mat_<double>(3, 1) << 2823.49, 2469.01, -212810);//座標変換用並進ベクトル
Mat rvec4 = (Mat_<double>(3, 1) << -0.0180059, -0.027595, -3.13899);
Mat tvec4 = (Mat_<double>(3, 1) << 2513.54, 2598.58, -219445);//座標変換用並進ベクトル
Mat rmat4;//座標変換用回転行列

Mat man1_pos2D(1, 3, CV_64FC1);
Mat man1_pos3D(1, 3, CV_64FC1);
Mat man2_pos2D(1, 3, CV_64FC1);
Mat man2_pos3D(1, 3, CV_64FC1);
Mat man3_pos2D(1, 3, CV_64FC1);
Mat man3_pos3D(1, 3, CV_64FC1);
Mat man4_pos2D(1, 3, CV_64FC1);
Mat man4_pos3D(1, 3, CV_64FC1);

int man1_flag = 0;//マニピュレータの座標取得前：0、取得中：1 ～ point_numbe - 1、取得後：point_number
int man2_flag = 0;//マニピュレータの座標取得前：0、取得中：1 ～ point_numbe - 1、取得後：point_number
int man3_flag = 0;
int man4_flag = 0;
int set_flag = 1;//高さと焦点面の取得を行う場合:1
int active_flag = 0; //搬送時に中央にあるホールディングピペットのハンドルNo

//0108追加　カメラ座標→三次元座標の変換誤差
double CtoW_man1_dx = 0.0;
double CtoW_man1_dy = 0.0;
double CtoW_man1_dz = 0.0;
double temp_man1_pos3D[3];
double CtoW_man2_dx = 0.0;
double CtoW_man2_dy = 0.0;
double CtoW_man2_dz = 0.0;
double temp_man2_pos3D[3];
double CtoW_man3_dx = 0.0;
double CtoW_man3_dy = 0.0;
double CtoW_man3_dz = 0.0;
double temp_man3_pos3D[3];
double CtoW_man4_dx = 0.0;
double CtoW_man4_dy = 0.0;
double CtoW_man4_dz = 0.0;
double temp_man4_pos3D[3];

//変換確認用
double t_3Dpos[3];
double t_2Dpos[3] = { 300,300,0 };
double t_3Dpos_ex[3];
double t_2Dpos_ex[3] = { 300,300,0 };

double ex_normalx = 190.0;
double ex_normaly = 154.0;

double man1_edge_max = 0;
double man1_s_dWD2;
double man1_zc0 = 0;
double man1_zc;
double man2_edge_max = 0;
double man2_s_dWD2;
double man2_zc0 = 0;
double man2_zc;
double man3_edge_max = 0;
double man3_s_dWD2;
double man3_zc0 = 0;
double man3_zc;
double man4_edge_max = 0;
double man4_s_dWD2;
double man4_zc0 = 0;
double man4_zc;

double man1_dx = 0.0;
double man1_dy = 0.0;
double man2_dx = 0.0;
double man2_dy = 0.0;
double man3_dx = 0.0;
double man3_dy = 0.0;
double man4_dx = 0.0;
double man4_dy = 0.0;

int dia_man1 = 30;//man1の直径(pixel)
int dia_man2 = 160;//man2の直径(pixel)

int x0_M;
int y0_M;
double x1_M;
double y1_M;
double M0;
double M1;
double cx = IMG_WIDTH2 / 2;
double cy = IMG_HEIGHT2 / 2;
Mat back_img;
bool back_img_flag = false;
double d1_0;
double d1;		//レンズの主点からディッシュの底面までの距離(mm)
double d2 = 1.0;		//ディッシュの厚さ(mm)
double d3;		//ディッシュ上面から焦点面までの距離(mm)
double d4 = 16.711125;		//対物レンズの先端からディッシュの底面までの距離(mm)
double ng = 1.592;	//ディッシュの屈折率(ポリスチレン)
double nl = 1.33;	//液体の屈折率（純水）
double deltaObETOL_0;//対物レンズの先端からレンズの主点までの距離
double deltaObETOL;
double delta_f0;//光の屈折による焦点距離の変化量（プログラム開始時）
double delta_f1;//光の屈折による焦点距離の変化量（リアルタイム）
double delta_f2;
double delta_f3;

//半自動操作
double set_pos12d[3] = { 550,-400 };//スタンバイポジション(上)
double set_pos22d[3] = { 550,1400 };//スタンバイポジション(下)
double cent_pos12d[3] = { 550,504 };//中央作業時のピペット位置 :
double cent_pos22d[3] = { 550,504 };//中央作業時のピペット位置 :
double cent_pos13d[3];
double cent_pos23d[3];
double set_pos12d_ex[3] = { 0,0,0 }; //搬送では使わない　搬送の位置確認用
double set_pos22d_ex[3] = { 0,0,0 }; //搬送では使わない　搬送の位置確認用
double set_pos13d[3];
double set_pos23d[3];
int auto_flag = 0;
int auto_step = 0; //3の時：右上が中央 4の時：右下が中央
double man4_auto_z1 = -2350;
double man2_auto_z1 = -2350;
double man4_auto_z2 = -2500;
double man2_auto_z2 = -2500;
int move_count = 0;
int hold_flag2 = 0;//保持できていたら1, できてなかったら0 ピペット2
int hold_flag4 = 0;//保持できていたら1, できてなかったら0 ピペット4
int p_flag = 0; //0の時円検出先を半自動　1の時クリック先を半自動

/*モータ制御用*/
// ---- ボード ----
WORD	   DA_Data[4];							// DAボードの出力
//int      DA_Data[4];							// DAボードの出力
DASMPLCHREQ	DaSmplChReq[4];						// DAボードの出力範囲
int ChNo;										// ボードのチャンネル番号
int	Ret;
char szName[32];
HANDLE      hDeviceHandle_enc;					// カウンタボードのデバイスハンドル
DWORD inputda;
int rotation_count1; //現在の回転角度
int motor_flag = 0;
double target_angle[4] = { 0,0,0,0 }; //目標回転角度(単位は回転数)
WORD DA_Data_zero[4] = { 0x0080000 + 32248,0x0080000 + 32197,0x0080000 + 32238,0x0080000 + 32226 }; //モータのゼロ入力


//injector自動化用
/*タイマー関数*/
DWORD	dwUser;
DWORD	gdwTimerRunTime;	//タイマー使用開始からの時間[ms]
DWORD	gdwStartTime;		//開始時刻（windows起動時からの時間）
//int flag = 0;
int count2;
int set = 0;
void CALLBACK timerProc(UINT unTimerID, UINT unMsg, DWORD_PTR dwUser, DWORD_PTR dummy1, DWORD_PTR dummy2);

//----- カウンタボード用 -----
unsigned long dwCounter[4];
unsigned long dwChSel[4];
unsigned long dwCountMode;
unsigned long dwLoadMode;
unsigned long dwLatchMode;
unsigned long dwLoadData;

//----- 変数 -----
double mI[] = { 0,0,0,0 };
double e_sub[] = { 0,0,0,0 };
double target1 = 0;
double target2 = 0;
double error_revolution = 0;
double pump_speed[4];
double msec;
double th[] = { 0,0,0,0 };
double th_ini[] = { 0,0,0,0 };
double target_revolution(int ChNo, double left_angle);
double get_revolution(int ChNo);
double target_revolution_1[1000000];
double get_revolution_1[1000000];
double get_revolution_2[1000000];
double get_revolution_3[1000000];
double get_revolution_4[1000000];
double target_revolution_2[1000000];
double target_revolution_3[1000000];
double target_revolution_4[1000000];
int motor_rev = 1;
int motor_end_flag = 0;

double PID_angle(int num, double target_angle);
double t, tp = 0.0;
double dt;
int dc_n = 0;

//自動インジェクタ用　大島追加
#include "ModbusCVD.h"
#include <thread>
#include<conio.h>
// ... (グローバル領域)
#include <torch/script.h>
#include <deque>
#include <atomic>

// --- インクルード追加 ---
#include <queue>
#include <mutex>
#include <condition_variable>
// --- グローバル変数追加 ---
bool g_is_recording_active = false;
std::queue<cv::Mat> g_frame_queue;      // 画像を一時的に溜めるキュー
std::mutex g_queue_mutex;               // 排他制御用
std::condition_variable g_queue_cv;     // スレッド待機用
std::atomic<bool> g_recorder_thread_active(true); // スレッド生存フラグ
// 録画オブジェクト
cv::VideoWriter g_recorder;

// --- AI推論用のグローバル変数 ---
torch::jit::script::Module g_ai_model;
torch::Device g_ai_device = torch::cuda::is_available() ? torch::kCUDA : torch::kCPU;
const int G_AI_SEQ_LENGTH = 8; // (C++側では直接使わないが、参考のために残す)
const double G_AI_STOP_THRESHOLD = 0.95;

std::atomic<bool> g_ai_capture_request(false);
std::atomic<bool> g_ai_motor_engaged(false);
std::atomic<double> g_ai_progress(0.0);
std::atomic<bool> g_ai_is_stop(false);
std::atomic<double> g_ai_fps(0.0); // ★ AI推論のFPSを格納する
std::atomic<int> g_locked_roi_width(0);  // AI作動中に使用する「固定された」ROIの幅
std::atomic<int> g_locked_roi_height(0); // AI作動中に使用する「固定された」ROIの高さ
std::atomic<int> g_ai_stop_counter(0); // 連続STOPフレームのカウンター
// 既存の変数付近に追加
std::atomic<long> g_total_moved_steps(0);    // ★ 合計移動ステップ数記録用
std::atomic<int>  g_ai_drive_counter(0);     // ★ クラス0, 1用の連続検知カウンタ
std::atomic<int>  g_last_pred_class(-1);     // ★ 前回のクラス記憶用（切り替わり判定）
const long MAX_TOTAL_STEPS = 300; // ★ 安全装置の上限ステップ数
std::atomic<bool> g_ai_injection_done(false); // ★追加: 注入完了フラグ

// ★ 差分計算のための「最初のフレーム」を保持する (cv::Mat形式)
cv::Mat g_ai_initial_frame_mat;

// ★ AIのLSTM内部状態 (Hidden State, Cell State) を保持する
torch::Tensor g_ai_hidden_h;
torch::Tensor g_ai_hidden_c;

// (高速版では initial_frame と frame_history は不要なため削除)

// 正規化(Normalize)用テンソル
torch::Tensor g_mean_tensor = torch::tensor({ 0.485, 0.456, 0.406 }).view({ 3, 1, 1 });
torch::Tensor g_std_tensor = torch::tensor({ 0.229, 0.224, 0.225 }).view({ 3, 1, 1 });
//sleep関数
double sleep(double sleep_time);
/**
 * @brief AIモデルをファイルからロードする (main関数から呼び出す)
 */
void loadAIModel(const std::string& model_path) {
	try {
		// ★ 読み込むモデルファイルを新しい高速版に変更
		g_ai_model = torch::jit::load("streaming_2stream_model_cpp.pt");
		g_ai_model.to(g_ai_device);
		g_ai_model.eval();
		std::cout << "[AI] Model loaded successfully to " << g_ai_device << std::endl;
		g_mean_tensor = g_mean_tensor.to(g_ai_device);
		g_std_tensor = g_std_tensor.to(g_ai_device);
	}
	catch (const c10::Error& e) {
		std::cerr << "[AI] CRITICAL: Error loading model: " << e.what() << std::endl;
	}
}

/**
 * @brief OpenCVのMatをAIモデル用のTensorに前処理する
 */
torch::Tensor ai_preprocess(const cv::Mat& frame_bgr) {

	const int TARGET_SIZE = 224; // 最終的なサイズ (224x224)

	// 1. BGR -> RGB
	cv::Mat frame_rgb;
	cv::cvtColor(frame_bgr, frame_rgb, cv::COLOR_BGR2RGB);

	// 2. ★ 単純リサイズ (LongestMaxSize)
	cv::Mat resized_frame;
	cv::resize(frame_rgb, resized_frame, cv::Size(TARGET_SIZE, TARGET_SIZE));

	// 3. ToTensor (HWC -> CHW, 0-255 -> 0.0-1.0)
	torch::Tensor tensor_image = torch::from_blob(
		resized_frame.data,
		{ TARGET_SIZE, TARGET_SIZE, 3 },
		torch::kByte
	);

	tensor_image = tensor_image.permute({ 2, 0, 1 });
	tensor_image = tensor_image.to(torch::kFloat32).div(255.0);

	// 4. Normalize
	tensor_image = tensor_image.sub(g_mean_tensor).div(g_std_tensor);
	return tensor_image.to(g_ai_device);
}

//自動インジェクタモータ制御用の関数とグローバル変数　
// Modbus通信とモーターオブジェクトをグローバル変数として宣言
modbus_t* sharedCtx = nullptr;
ModbusCVD* motor1 = nullptr;
ModbusCVD* motor2 = nullptr;

// 各モーターの現在の運転状態（速度と方向）を保持する変数
int current_speed1 = 0;
int current_direction1 = 0;
int current_speed2 = 0;
int current_direction2 = 0;
// 相対位置決め設定値（初期値）
int g_step_count = 20;  // 1回あたりの移動ステップ数
int g_step_speed = 100;
int g_step_speed_2 = 100;
// 移動速度

// モーターを動かす・停止するためのラッパー関数
void move(ModbusCVD& motor, int speed, int direction, const std::string& motorName) {
	if (!motor.moveMotor(speed, direction, motorName)) {
		std::cerr << "Failed to control " << motorName << std::endl;
	}
}

void stop(ModbusCVD& motor, const std::string& motorName) {
	if (!motor.stopMotor()) {
		std::cerr << "Failed to stop " << motorName << std::endl;
	}
}
// ======================
//  入力用・制御用関数の追加
// ======================
// コンソールから数値を入力するためのヘルパー関数
// 戻り値: 入力された数値 (キャンセルされた場合は -1 を返す)
int getInputValueFromConsole(const std::string& promptName, int currentValue) {
	std::string inputBuffer = "";
	char ch = 0;
	// 現在の設定値を表示して入力を促す
	std::cout << "\n========================================" << std::endl;
	std::cout << " [INPUT MODE] Change " << promptName << std::endl;
	std::cout << " Current Value: " << currentValue << std::endl;
	std::cout << " Type new value and press ENTER (ESC to cancel): ";
	// 入力ループ
	while (true) {
		if (_kbhit()) { // キー入力があるか確認
			ch = _getch(); // 1文字読み取り（画面には表示されない）
			// Enterキー (CR: 13) -> 確定
			if (ch == 13) {
				std::cout << std::endl; // 改行
				break;
			}
			// ESCキー (27) -> キャンセル
			else if (ch == 27) {
				std::cout << "\n[Canceled]" << std::endl;
				std::cout << "========================================\n" << std::endl;
				return -1;
			}
			// Backspaceキー (8) -> 文字削除
			else if (ch == 8) {
				if (!inputBuffer.empty()) {
					inputBuffer.pop_back();
					std::cout << "\b \b"; // 画面上の文字も消す
				}
			}
			// 数字のみ受け付ける
			else if (isdigit(ch)) {
				inputBuffer += ch;
				std::cout << ch; // 入力文字を表示
			}
		}
		sleep(10); // CPU負荷軽減
	}
	if (inputBuffer.empty()) return -1; // 空エンターの場合は変更なし
	try {
		int newValue = std::stoi(inputBuffer);
		std::cout << " >> New Value Set: " << newValue << std::endl;
		std::cout << "========================================\n" << std::endl;
		return newValue;
	}
	catch (...) {
		std::cerr << " Invalid Number!" << std::endl;
		return -1;
	}
}
// 相対位置決め運転（指定ステップ数だけ移動）を行う関数
// steps: 正の値で正転、負の値で逆転
void moveStepRelative(ModbusCVD& motor, int steps, int speed, const std::string& motorName) {
	printf("[%s] - Starting relative move: %d steps at speed %d...\n", motorName.c_str(), steps, speed);

	// 1. アラームが出ていたらリセット
	if (!motor.resetAlarm()) {
		std::cerr << motorName << " - Failed to reset alarm." << std::endl;
		return;
	}

	// 2. FREE（脱磁）状態を解除して、モータを動かせる状態（励磁）にする
	if (!motor.free(false)) {
		std::cerr << motorName << " - Failed to enable motor (free off)." << std::endl;
		return;
	}

	// 3. ModbusCVD.cpp の startPosition を呼び出す
	// 第3引数の "2" は相対位置決めモード（1=絶対, 2=相対）
	if (motor.startPosition(steps, speed, 2)) {
		printf("[%s] - Command sent successfully.\n", motorName.c_str());
	}
	else {
		std::cerr << motorName << " - startPosition command failed." << std::endl;
	}
}
void recordingThreadFunc() {
	int frame_count = 0;

	while (g_recorder_thread_active.load()) {
		cv::Mat frame; // 名前をシンプルに
		{
			std::unique_lock<std::mutex> lock(g_queue_mutex);
			g_queue_cv.wait(lock, [] {
				return !g_frame_queue.empty() || !g_recorder_thread_active.load();
				});
			if (!g_recorder_thread_active.load() && g_frame_queue.empty()) break;
			if (!g_frame_queue.empty()) {
				frame = g_frame_queue.front();
				g_frame_queue.pop();
			}
		}
		if (frame.empty()) {
			// 終了合図
			if (g_recorder.isOpened()) {
				g_recorder.release();
				std::cout << "[REC] Worker: Saved & Closed. Total Frames: " << frame_count << std::endl;
			}
		}
		else {
			if (g_recorder.isOpened()) {
				try {
					// ▼▼▼【重要】強制カラー化ロジック ▼▼▼
					// 白黒(Type 0)なら、自分自身をカラーに変換して上書きする
					if (frame.type() == CV_8UC1) {
						cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
					}
					// ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲
					// 最初のフレームだけ情報を表示（必ず frame.type() を見る）
					if (frame_count == 0) {
						std::cout << "[DEBUG] Write Info -> "
							<< "Size:" << frame.cols << "x" << frame.rows
							<< " Type:" << frame.type() << std::endl;
						// ★ここが「16」になれば勝利確定です
					}
					g_recorder.write(frame);
					frame_count++;

					if (frame_count % 10 == 0) std::cout << ".";
				}
				catch (const std::exception& e) {
					std::cout << "[ERROR] Write failed: " << e.what() << std::endl;
				}
			}
		}
	}
}
//追加ここまで

char start;

//データ保存
ofstream data_file1;
ofstream data_file2;
char fname[300];
int file_number = 1;
bool file_flag = false;
double counter_time = 0;

class Timer_function
{
	// timeGetTime() を使った時間計測タイマー クラス
	//#pragma comment(lib, "winmm.lib")
public:
	Timer_function() { restart(); }
public:
	void  restart()
	{
		m_start = timeGetTime();        // 計測開始時間を保存
	}
	double  elapsed()    // リスタートからの秒数を返す
	{
		DWORD end = timeGetTime();
		return (double)(end - m_start) / 1000;
	}
private:
	DWORD    m_start;    //  計測開始時間
};

bool super_resolution(const Mat& input_image, Mat& output_image, const string& model_path) {
	try {
		// DnnSuperResImplの作成
		cv::dnn_superres::DnnSuperResImpl sr;

		// モデルの読み込み
		sr.readModel(model_path);

		// モデルの設定（LapSRN, スケール4倍）
		sr.setModel("fsrcnn", 4);

		// 超解像処理の実行
		sr.upsample(input_image, output_image);

		return true;
	}
	catch (const Exception& e) {
		cerr << "Error: " << e.what() << endl;
		return false;
	}
}

void act_output_DA(void) { //アナログ出力
	int	Ret;
	Ret = DaOutputDA(ghDeviceHandle_da2, 4, &DaSmplChReq[0], &DA_Data[0]);						// 各チャンネルごとに電圧を出力
	//printf("actoutputDA\n");
	if (Ret != DA_ERROR_SUCCESS) {
		printf("DaOutputDA errr(%lx)\n", Ret);
	}
}

void zero_output_DA(void) { //0Vになるような値をセット
	DA_Data[0] = DA_Data_zero[0];			  // DA1のCH1に0Vを設定
	DA_Data[1] = DA_Data_zero[1];			  // DA2のCH1に0Vを設定
	DA_Data[2] = DA_Data_zero[2];			  // DA3のCH1に0Vを設定
	DA_Data[3] = DA_Data_zero[3];			  // DA3のCH1に0Vを設定
	//printf("zerooutputDA\n");
}

bool validateImage2(const cv::Mat& img, std::string& error_message) {
	try {
		// 1. 画像が空でないかチェック
		if (img.empty()) {
			error_message = "Input image is empty";
			return false;
		}

		// 2. メモリ確保チェック
		if (!img.data) {
			error_message = "Failed to allocate memory for the image";
			return false;
		}

		// 3. 画像サイズチェック
		if (img.rows <= 0 || img.cols <= 0) {
			error_message = "Invalid image dimensions: " +
				std::to_string(img.rows) + "x" +
				std::to_string(img.cols);
			return false;
		}

		// 4. 画像サイズの上限チェック
		const int MAX_IMAGE_SIZE = 10000;  // 適切な上限値に調整してください
		if (img.rows > MAX_IMAGE_SIZE || img.cols > MAX_IMAGE_SIZE) {
			error_message = "Image dimensions exceed maximum allowed size (" +
				std::to_string(MAX_IMAGE_SIZE) + ")";
			return false;
		}

		// 5. データ型チェック
		if (img.depth() != CV_8U && img.depth() != CV_8S &&
			img.depth() != CV_16U && img.depth() != CV_16S &&
			img.depth() != CV_32F && img.depth() != CV_64F) {
			error_message = "Invalid image data type: " +
				std::to_string(img.depth());
			return false;
		}

		// 6. チャンネル数チェック
		if (img.channels() != 1 && img.channels() != 3 && img.channels() != 4) {
			error_message = "Invalid number of channels: " +
				std::to_string(img.channels());
			return false;
		}

		// 7. ピクセル値の範囲チェック（8ビット画像の場合）
		if (img.depth() == CV_8U) {
			double minVal, maxVal;
			cv::minMaxLoc(img, &minVal, &maxVal);
			if (maxVal > 255 || minVal < 0) {
				error_message = "Pixel values out of valid range [0,255]";
				return false;
			}
		}

		// 8. データの連続性チェック（警告のみ）
		if (!img.isContinuous()) {
			std::cout << "Warning: Image data is not continuous" << std::endl;
		}

		return true;

	}
	catch (const cv::Exception& e) {
		error_message = "OpenCV error: " + std::string(e.what());
		return false;
	}
	catch (const std::exception& e) {
		error_message = "Standard error: " + std::string(e.what());
		return false;
	}
	catch (...) {
		error_message = "Unknown error occurred";
		return false;
	}
}


//void act_output_DA1(void) {
//	int	Ret;
//	Ret = DaOutputDA(ghDeviceHandle_da2, 1, &gstrDaSmplChReq[0], &DA_Data[0]);						// 各チャンネルごとに電圧を出力
//
//	if (Ret != DA_ERROR_SUCCESS) {
//		printf("DaOutputDA errr(%lx)\n", Ret);
//	}
//
//}
//void act_output_DA_key1(int DA_Data) {
//	int	Ret;
//	Ret = DaOutputDA(ghDeviceHandle_da2, 1, &gstrDaSmplChReq[0], &DA_Data);						// 各チャンネルごとに電圧を出力
//
//	if (Ret != DA_ERROR_SUCCESS) {
//		printf("DaOutputDA errr(%lx)\n", Ret);
//	}
//}

//void zero_output_DA1(void) {
//	DA_Data[0] = 0x0080000 + 32248;		        // DA2のCH1に0Vを設定
//	int	Ret;
//	Ret = DaOutputDA(ghDeviceHandle_da2, 1, &gstrDaSmplChReq[0], &DA_Data[0]);						// 各チャンネルごとに電圧を出力
//
//	if (Ret != DA_ERROR_SUCCESS) {
//		printf("DaOutputDA errr(%lx)\n", Ret);
//	}
//}

//void act_output_DA2(void) {
//	int	Ret;
//	Ret = DaOutputDA(ghDeviceHandle_da2, 1, &gstrDaSmplChReq[1], &DA_Data[1]);						// 各チャンネルごとに電圧を出力
//
//	if (Ret != DA_ERROR_SUCCESS) {
//		printf("DaOutputDA errr(%lx)\n", Ret);
//	}
//}

//void act_output_DA_key2(int DA_Data) {
//	int	Ret;
//	Ret = DaOutputDA(ghDeviceHandle_da2, 1, &gstrDaSmplChReq[1], &DA_Data);						// 各チャンネルごとに電圧を出力
//
//	if (Ret != DA_ERROR_SUCCESS) {
//		printf("DaOutputDA errr(%lx)\n", Ret);
//	}
//}

//void zero_output_DA2(void) {
//	DA_Data[1] = 0x0080000 + 32348;			  // DA2のCH1に0Vを設定
//	int	Ret;
//	Ret = DaOutputDA(ghDeviceHandle_da2, 1, &gstrDaSmplChReq[1], &DA_Data[1]);						// 各チャンネルごとに電圧を出力
//
//	if (Ret != DA_ERROR_SUCCESS) {
//		printf("DaOutputDA errr(%lx)\n", Ret);
//	}
//}

//void act_output_DA3(void) {
//	int	Ret;
//	Ret = DaOutputDA(ghDeviceHandle_da2, 1, &gstrDaSmplChReq[2], &DA_Data[2]);						// 各チャンネルごとに電圧を出力
//
//	if (Ret != DA_ERROR_SUCCESS) {
//		printf("DaOutputDA errr(%lx)\n", Ret);
//	}
//}

//void act_output_DA_key3(int DA_Data) {
//	int	Ret;
//	Ret = DaOutputDA(ghDeviceHandle_da2, 1, &gstrDaSmplChReq[ChNo], &DA_Data);						// 各チャンネルごとに電圧を出力
//
//	if (Ret != DA_ERROR_SUCCESS) {
//		printf("DaOutputDA errr(%lx)\n", Ret);
//	}
//}

//void zero_output_DA3(void) {
//	DA_Data[2] = 0x0080000 + 32238;			  // DA3のCH1に0Vを設定
//	int	Ret;
//	Ret = DaOutputDA(ghDeviceHandle_da2, 1, &gstrDaSmplChReq[2], &DA_Data[2]);						// 各チャンネルごとに電圧を出力
//
//	if (Ret != DA_ERROR_SUCCESS) {
//		printf("DaOutputDA errr(%lx)\n", Ret);
//	}
//}

//void act_output_DA4(void) {
//	int	Ret;
//	Ret = DaOutputDA(ghDeviceHandle_da2, 1, &gstrDaSmplChReq[3], &DA_Data[3]);						// 各チャンネルごとに電圧を出力
//
//	if (Ret != DA_ERROR_SUCCESS) {
//		printf("DaOutputDA errr(%lx)\n", Ret);
//	}
//}

void act_output_DA_key(int DA_Data, int ChNo) {
	int	Ret;
	Ret = DaOutputDA(ghDeviceHandle_da2, 1, &gstrDaSmplChReq[ChNo], &DA_Data);						// 各チャンネルごとに電圧を出力

	if (Ret != DA_ERROR_SUCCESS) {
		printf("DaOutputDA errr(%lx)\n", Ret);
	}
}

//void zero_output_DA4(void) {
//	DA_Data[3] = 0x0080000 + 32245;			  // DA3のCH1に0Vを設定
//	int	Ret;
//	Ret = DaOutputDA(ghDeviceHandle_da2, 1, &gstrDaSmplChReq[3], &DA_Data[3]);						// 各チャンネルごとに電圧を出力
//
//	if (Ret != DA_ERROR_SUCCESS) {
//		printf("DaOutputDA errr(%lx)\n", Ret);
//	}
//}

void get_count_ini(int ChNo) {
	unsigned long	count;
	double tttt;
	Ret = UcntReadCounter(hDeviceHandle_enc, dwChSel[ChNo], dwCounter);
	/*Ret = UcntReadCounter(hDeviceHandle_enc, dwChSel[ChNo], &dwCounter[0]);*/ //各チャンネル毎のカウンタ値を取得
	if (Ret != IFUCNT_ERROR_SUCCESS) {
		printf("UcntReadCounter errr(%lx)", Ret);
	}
	count = dwCounter[ChNo];
	th_ini[ChNo] = ((double)count) / 8192.0;//200000 一回転で何パルスか
	printf("th_ini = %f ChNo = %d \n", th_ini[ChNo], ChNo);
}

void set_DA(double target_angle[4]) {
	DA_Data[0] = DA_Data_zero[0] + PID_angle(0, target_angle[0]);
	DA_Data[1] = DA_Data_zero[1] + PID_angle(1, target_angle[1]);
	DA_Data[2] = DA_Data_zero[2] + PID_angle(2, target_angle[2]);
	DA_Data[3] = DA_Data_zero[3] + PID_angle(3, target_angle[3]);
	//DA_Data = 0x0080000 + 32768 + PID_angle(0);
}

void counterborad(void) {
	// Configure the pulse counter mode.
   // When the phase-shifted pulse count mode and phase Z are enabled, the counter clear and no counter latch.
	//for (ChNo = 1; ChNo < 5; ChNo++) {
	//for (ChNo = 3; ChNo < 4; ChNo++) {
	for (ChNo = 1; ChNo < 5; ChNo++) {
		Ret = UcntSetPulseCountMode(hDeviceHandle_enc, ChNo, dwCountMode, dwLoadMode, dwLatchMode);
		if (Ret != IFUCNT_ERROR_SUCCESS) {
			printf("SetPulseCountMode=%Xh\n", Ret);				// エラーコードを表示
		}

		// Configure the counter value to the load counter.
		dwLoadData = 0;
		Ret = UcntSetLoadData(hDeviceHandle_enc, ChNo, dwLoadData);
		if (Ret != IFUCNT_ERROR_SUCCESS) {
			printf("UcntSetLoadData=%Xh\n", Ret);					// エラーコードを表示
		}
		// Start the count.
		dwChSel[(ChNo - 1)] = 0x01 << (ChNo - 1);
		Ret = UcntStartCount(hDeviceHandle_enc, dwChSel[(ChNo - 1)], IFUCNT_CMD_START);
		if (Ret != IFUCNT_ERROR_SUCCESS) {
			printf("UcntStartCount=%Xh\n", Ret);					// エラーコードを表示
		}
	}
	//}
}

double get_revolution(int ChNo) {//初期から動いた角度　最終的にtarget_revolutionと一致
	unsigned long dwCounter[4];
	int				Ret;
	unsigned long	count;

	Ret = UcntReadCounter(hDeviceHandle_enc, dwChSel[ChNo], dwCounter); //各チャンネル毎のカウンタ値を取得
	if (Ret != IFUCNT_ERROR_SUCCESS) {
		printf("UcntReadCounter errr(%lx)", Ret);
	}
	count = dwCounter[ChNo];
	//cout << "count_now = " << (double)count / 200000.0 - th_ini[ChNo] << endl;
	th[ChNo] = (((double)count) / 8192.0) - th_ini[ChNo];

	return th[ChNo];
}

void return_ini_rev(int ChNo) {
	double f;
	//double kp = 4000.0, kd = 50.0, ki = 10.0; //kid制御定数 kp=50,kd=50,ki=3 1200 200 10
	double kp = 4000.0;
	//double kp = 50, kd = 50, ki = 3.0; //kid制御定数 kp=50,kd=50,ki=3 1200 200 10
	double e = 0 - (-get_revolution(ChNo));
	double d_1 = (e - e_sub[ChNo]) / 1000;
	mI[ChNo] = mI[ChNo] + e / 1000;
	//f = kp * e + kd * d_1 + ki * mI[0];
	f = kp * e;
	if (f >= 10000.0)f = 10000.0;
	else if (f < -10000.0)f = -10000.0;
	e_sub[ChNo] = e;
	//DA_Data[0] = 32048 + f;
	DA_Data[0] = 32623 + f;

	act_output_DA();
}

//手の角度からモーターの回転角度
double target_revolution(int ChNo, double positive_angle) {
	double rev;
	double inj_rev;
	double pitch = 1.0;
	double rev_k = 1.0;
	//rev = target_pos(ChNo) / pitch;
	inj_rev = -(positive_angle / 60) * rev_k; //角度からインジェクターの回転数へ変換 インジェクター1回転当たりの把持角度
	rev = inj_rev * 1.8148; //インジェクター回転数からモーター回転数に変換　減速比1.8148
	//cout << "target_revolution = " << rev << endl;

	//rev = 0;
	return rev;
}

double PID_angle(int num, double target_angle) {
	double f;
	//double kp = 7000000.0, kd = 7000000.0, ki = 0.0;
	double kp = 3000.0, kd = 100, ki = 0;
	//double kp = 4000.0,kd=400;
	error_revolution = target_angle - (-get_revolution(num));
	//cout << "error_revolution = " << error_revolution << endl;
	double d_1 = (error_revolution - e_sub[num]) / 1000;
	mI[num] = mI[num] + error_revolution / 1000;
	//f = kp * error_revolution + kd * d_1 + ki * mI[num];
	f = kp * error_revolution + kd * d_1;
	if (f >= 10000.0)f = 10000.0;
	else if (f < -10000.0)f = -10000.0;
	e_sub[num] = error_revolution;
	//cout << "error_revolution = " << error_revolution << endl;
	return f;
}

std::mutex mtx1;

void CALLBACK timerProc(UINT unTimerID, UINT unMsg, DWORD_PTR dwUser, DWORD_PTR dummy1, DWORD_PTR dummy2) {
	if (motor_rev == 0) {
		set_DA(target_angle);
		act_output_DA();
		//act_output_DA1();
		//act_output_DA2();
		//act_output_DA3();
		//act_output_DA4();
		//d_t[set] = t;
		//set = set + 1;
		(*(int*)dwUser)++;//count1を1つ増やす
		//t = t + 0.001;
	}

	if (motor_rev == 1) {
		//return_ini_rev();
		zero_output_DA();
		act_output_DA();
	}

	t = t + 0.001;//こっちに変更
}

typedef struct test1 {
	int monitor_width = GetSystemMetrics(SM_CXSCREEN);
	int monitor_height = GetSystemMetrics(SM_CYSCREEN);
	int switch_window_state = 0;		//ウィンドウの切替を行う際に、この値で切替先を指定する。 0:視野拡張画像全体更新、1:視野拡張画像上半分更新、2：単一画像表示、3:視野拡張画像下半分更新
	int switch_window_state_tmp;		//値保持用
	BOOL switch_window_flag = FALSE;	//表示する画像を変更する際に，make_angle_table2で計算を行わせるかの判定（TRUE:画像の切り替え，FALSE:現在の画像を表示し続ける）※更新する領域自体を変更する際に使用
	BOOL select_window = FALSE;			//画像を選択する状態にあるか．FALSE:拡張画像表示，TRUE:選択画像表示		(man2停止時に選択を行うモードで使用)
	BOOL command_select_window = TRUE;	//選択画像の表示を行うか操作側の入力で切替を行う。(TRUE:選択を行う、FALSE:選択を行わない)
	BOOL view_expandedimg = TRUE;		//拡張画像を分割せず表示する(TRUE)か，拡張画像を上下に分割して表示する(FALSE)か
	BOOL start_calib_flag = TRUE;		//キャリブレーションを行うかどうかの判定（最初にキャリブレーションを行う必要があるので初期値はTRUE）。C&Y、C&Nで切替。スレッド１と2で使用
	BOOL gamma_view_flag = FALSE;	//ガンマ補正を行うかどうかの判定。G&Y、G&Nで切替。 transport_multi()内で使用
	BOOL display_gaze_point = TRUE;		//視線を画面上に表示
	BOOL calib_single = FALSE;			//キャリブレーションを行う画像（FALSE:拡張画像、TRUE:単一画像）
	BOOL test = FALSE;
}WINDOW_STATES;

double IMG_HEIGHT_ex;

//拡張画像から通常視野の座標に変換 x
double ex_to_normalx(double pos_ex) {
	double pos;
	pos = 623.0 + (623.0 / (b_framex / 2)) * (pos_ex - 623.0);
	return pos;
}

//拡張画像から通常視野の座標に変換 y
double ex_to_normaly(double pos_ex) {
	double pos;
	pos = 504.0 + (1008 / b_framey) * (pos_ex - ((frame_height * IMG_WIDTH * MAG_IMG / frame_width) / 2));
	return pos;
}

//通常視野から拡張視野の座標に変換x
double normal_to_exx(double pos) {
	double pos_ex;
	pos_ex = 623.0 + ((b_framex / 2) / 623.0) * (pos - 623.0);
	return pos_ex;
}

//通常視野から拡張視野の座標に変換y
double normal_to_exy(double pos) {
	double pos_ex;
	pos_ex = ((frame_height * IMG_WIDTH * MAG_IMG / frame_width) / 2) + (b_framey / 1008) * (pos - 504.0);
	return pos_ex;
}

WINDOW_STATES window_states;

double frame_pxl_width = IMG_WIDTH * MAG_IMG;		//IMG_WIDTH * MAG_IMG	1280	//モニター上に表示されるフレーム幅
double frame_pxl_height = IMG_HEIGHT * MAG_IMG;		//IMG_HEIGHT * MAG_IMG	1008		//モニター上に表示されるフレーム高さ
double windowframe_xpos = (window_states.monitor_width - frame_pxl_width) / 2;		//ウィンドウを中央に配置するための定数(モニタサイズに依存しない) (1920-1280)/2 = 320
double windowframe_ypos = (window_states.monitor_height - frame_pxl_height) / 2;	//ウィンドウを中央に配置するための定数(モニタサイズに依存しない) (1080-960)/2 = 60

//opticalflowに必要なもの
UINT32 nFramerate;
UINT32 nShutterspeed;

int End(HANDLE hDeviceHandle_ad)
{
	// デバイスのクローズ
	AdClose(hDeviceHandle_ad);

	return 0;
}

//マウスからの入力を取得
void mouse_callback(int event, int x, int y, int flags, void* userdata) {
	bool* isClick = static_cast<bool*>(userdata);
	*isClick = true;
	int center_framex2 = center_framex;
	int center_framey2 = center_framey;
	double MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;//最初の視野と現在の視野の割合
	double magnification2 = magnification;
	if (event == EVENT_RBUTTONUP) {	//右クリックした時（選択画像解除処理）
		t_pointed_count = pointed_count;
		phantom_flag = 1;
		for (int npointed_count = 0; npointed_count < pointed_count; npointed_count++) {
			//doubleにキャストしないとオーバーフローして選択解除できなくなる（IMG_WとIMG_Hが小さい場合はキャストしなくてもオーバーフローしない）
			if (pointed_pixelx[npointed_count] - double(IMG_WIDTH) / 2 * double(IMG_WIDTH) * MAG_IMG / MAG * pixelpitch / resolution / magnification2 / frame_width<x &&
				pointed_pixelx[npointed_count] + double(IMG_WIDTH) / 2 * double(IMG_WIDTH) * MAG_IMG / MAG * pixelpitch / resolution / magnification2 / frame_width>x &&
				pointed_pixely[npointed_count] - double(IMG_HEIGHT) / 2 * frame_height * double(IMG_WIDTH) * MAG_IMG / MAG * pixelpitch / resolution / magnification2 / frame_width / frame_height<y &&
				pointed_pixely[npointed_count] + double(IMG_HEIGHT) / 2 * frame_height * double(IMG_WIDTH) * MAG_IMG / MAG * pixelpitch / resolution / magnification2 / frame_width / frame_height>y) {
				if (npointed_count < pointed_count - 1) {
					for (int npointed_count2 = npointed_count; npointed_count2 < pointed_count - 1; npointed_count2++) {
						pointed_pixelx[npointed_count2] = pointed_pixelx[npointed_count2 + 1];
						pointed_pixely[npointed_count2] = pointed_pixely[npointed_count2 + 1];
						pointed_mirrorx[npointed_count2] = pointed_mirrorx[npointed_count2 + 1];
						pointed_mirrory[npointed_count2] = pointed_mirrory[npointed_count2 + 1];
						pointed_WD[npointed_count2] = pointed_WD[npointed_count2 + 1];
					}
					npointed_count--;
					pointed_count--;
				}
				if (npointed_count == pointed_count - 1) {
					npointed_count--;
					pointed_count--;
				}

			}


		}
		center_flag = 0;
		//cout << " end   position (x, y) : " << x << ", " << y << endl;

	}

	if (event == EVENT_LBUTTONUP && pointed_count == 0 && GetAsyncKeyState(0x48)) { //左クリック+Hボタン インジェクション位置反映
	//if (event == EVENT_LBUTTONUP && pointed_count == 0 && GetAsyncKeyState(0x48)) { //左クリック+Hボタン
		//*isClick = false;
		phantom_flag = 2;
		//インジェクションピペット周辺の切り抜き
		pointed_pixelx[pointed_count] = x;
		pointed_pixely[pointed_count] = y;

		//pointed_pixelx[pointed_count] = x;
		//pointed_pixely[pointed_count] = y;		
		pointed_mirrorx[pointed_count] = -atan((double)((x - IMG_WIDTH * MAG_IMG / 2) * MAG * resolution * frame_width / (IMG_WIDTH * MAG_IMG) + (double(center_framex2) - frame_width / 2) * resolution) / ((l2 + l3) * 1000 - z_in)) / 2 * 180 / PI;
		pointed_mirrory[pointed_count] = atan((double)((y - frame_height * double(IMG_WIDTH) * MAG_IMG / frame_width / 2) * MAG * resolution * frame_height / (frame_height * double(IMG_WIDTH) * MAG_IMG / frame_width) + (double(center_framey2) - frame_height / 2) * resolution) / ((l3) * 1000 - z_in)) / 2 * 180 / PI;
		pointed_WD[pointed_count] = derive_WD(pointed_mirrorx[pointed_count] * 2, pointed_mirrory[pointed_count] * 2);
		t_pointed_count = pointed_count;
		pointed_count++;
		if (pointed_count > max_pointed_count) pointed_count = 0;

		for (int npointed_count = 0; npointed_count < pointed_count; npointed_count++) {

			if (pointed_WD[npointed_count] > pointed_WD[pointed_count - 1]) {
				double a_line, b_line, c_line, d_line, e_line;
				a_line = pointed_pixelx[pointed_count - 1];
				b_line = pointed_pixely[pointed_count - 1];
				c_line = pointed_mirrorx[pointed_count - 1];
				d_line = pointed_mirrory[pointed_count - 1];
				e_line = pointed_WD[pointed_count - 1];
				pointed_pixelx[pointed_count - 1] = pointed_pixelx[npointed_count];
				pointed_pixely[pointed_count - 1] = pointed_pixely[npointed_count];
				pointed_mirrorx[pointed_count - 1] = pointed_mirrorx[npointed_count];
				pointed_mirrory[pointed_count - 1] = pointed_mirrory[npointed_count];
				pointed_WD[pointed_count - 1] = pointed_WD[npointed_count];
				pointed_pixelx[npointed_count] = a_line;
				pointed_pixely[npointed_count] = b_line;
				pointed_mirrorx[npointed_count] = c_line;
				pointed_mirrory[npointed_count] = d_line;
				pointed_WD[npointed_count] = e_line;
			}
		}
	}

	//cout << "center_flag=" << center_flag << endl;

	if (event == EVENT_MOUSEWHEEL) {
		int delta = getMouseWheelDelta(flags);
		printf("delta=%d\n", delta);

		MAG_ExpandedIMG = MAG_ExpandedIMG - dMAG * delta;
		if (MAG_ExpandedIMG > s_MAG_ExpandedIMG)MAG_ExpandedIMG = s_MAG_ExpandedIMG;
		else if (MAG_ExpandedIMG < min_MAG_ExpandedIMG)MAG_ExpandedIMG = min_MAG_ExpandedIMG;
		cout << "Mouse wheel is backward." << endl;
		ClTest.mouse_wheel_event = TRUE;
		printf("MAG=%f\n", MAG_ExpandedIMG);
	}

	if (event == EVENT_MOUSEMOVE) {
		mouse_posx = (x - (IMG_WIDTH * MAG_IMG) / 2) * frame_width / IMG_WIDTH / MAG_IMG;
		mouse_posy = (y - (frame_height * IMG_WIDTH * MAG_IMG / frame_width) / 2) * frame_width / IMG_WIDTH / MAG_IMG;

	}
	if (event == EVENT_LBUTTONUP && pointed_count == 0 && GetAsyncKeyState(0x4B)) { //左クリック+Kボタン
		if (p_flag == 0) {
			p_flag = 1;
			t_2Dpos_ex[0] = x;
			t_2Dpos_ex[1] = y;
		}
		else if (p_flag == 1) {
			p_flag = 0;
			t_2Dpos_ex[0] = 0;
			t_2Dpos_ex[1] = 0;
		}
		cout << "	t_2Dpos_exx = " << t_2Dpos_ex[0] << endl;
		cout << "	t_2Dpos_exy = " << t_2Dpos_ex[1] << endl;
		//拡張画像から通常視野の座標に変換
		t_2Dpos[0] = ex_to_normalx(t_2Dpos_ex[0]);
		t_2Dpos[1] = ex_to_normaly(t_2Dpos_ex[1]);

		cout << "	t_2Dpos_x = " << t_2Dpos[0] << endl;
		cout << "	t_2Dpos_y = " << t_2Dpos[1] << endl;
		//cout << "pposition=" << pPosition << endl;

	}
}

//ETLへの電流値の計算
void calculate_etl2(double dWD) {
	a_d = (dWD + f_Ob) * (-f_Ob + lOL_ObL - fOL) - lOL_ObL * f_Ob + f_Ob * fOL;
	b_d = (dWD + f_Ob) * ((fOL + b_f) * (f_Ob - lOL_ObL) + fOL * b_f) + lOL_ObL * f_Ob * (b_f + fOL) - f_Ob * fOL * b_f;
	c_d = (dWD + f_Ob) * ((a_f - b_f * fOL) * (f_Ob - lOL_ObL) + fOL * a_f) + lOL_ObL * f_Ob * (a_f - b_f * fOL) - f_Ob * fOL * a_f;
	d = (-b_d - sqrt(b_d * b_d - 4 * a_d * c_d)) / 2 / a_d;
	fETL = a_f / (d - b_f);
	fETOL = fETL * fOL / (fETL + fOL - d);
	fObETOL = fETOL * f_Ob / (fETOL + f_Ob - lOL_ObL);
	magnification = fTL / fObETOL;
	//printf("fETL=%f,fObETOL=%f,magnification=%f\n",fETL,fObETOL,magnification);
	//current = 13.226/*54*/ * ((1000 / fETL - 0.085 * (ETL_Temp - 30)) / (1 - (ETL_Temp - 30) * 0.002) - 4.9); //ETLのデータ表より導出
	current = 33.5/*54*/ * (1000 / fETL - 0.0752 * (ETL_Temp - 30) - 4.9) / (1 - (ETL_Temp - 30) * 0.002); //ETLのデータ表より導出 係数を39.8から変更
	// 参考https://www.optotune.com/images/products/Optotune%20EL-10-30.pdf　図６、１１
   //　参考https://www.optotune.com/images/products/Optotune%20Lens%20Driver%204%20manual.pdf　図１０
	//dpt = 1000 / (fETOL);
	//current = 300 / 9.04*(dpt+2.5);
	//etl = (dpt - dpt_min) / (dpt_max - dpt_min) * 5;
	//etl_current = (current - current_min) / (current_max - current_min) * 5;
	//cout << current << endl;
}

//random
int random(int low, int high)
{
	std::uniform_int_distribution<> dist(low, high);
	return dist(gen);
}

//sleep_time [ms]待機
double sleep(double sleep_time) {

	LARGE_INTEGER sleep_qpc1;
	LARGE_INTEGER sleep_qpc2;

	double Time = 0;
	QueryPerformanceCounter(&sleep_qpc1);
	while (Time < sleep_time) {
		QueryPerformanceCounter(&sleep_qpc2);
		Time = ((double)sleep_qpc2.QuadPart - (double)sleep_qpc1.QuadPart) / (double)qpf.QuadPart * 1000;
	}
	return Time;

}

void pipette_set_a() {
	double man3_set[3] = { 3650, -2850, -2200 };
	double man13_cent[3] = { 3350, -2500, -2250 };
	double man13_store[3] = { 0,0,0 };
	//Getposition(hDevice_RU, man13_store[0], man13_store[1], man13_store[2]);
	phase_flag = 2;
	move_flag = 0;
	move_mode = 0;
	//ピペットを移動
	std::vector<double> movepos3{ man3_set[0],man3_set[1],man3_set[2] }; //待機ポジ、後で決める
	Movepos(hDevice_RD, s, movepos3);
	man3_xw = man3_set[0];
	man3_yw = man3_set[1];
	man3_zw = man3_set[2];
	sleep(200);

	std::vector<double> movepos1{ man13_cent[0],man13_cent[1],man13_cent[2] + man1_auto_dz }; //待機ポジ、後で決める
	Movepos(hDevice_RU, s, movepos1);
	sleep(200);
	man1_xw = man13_cent[0];
	man1_yw = man13_cent[1];
	man1_zw = man13_cent[2] + man1_auto_dz;
	man1_xw0 = man1_xw - (pPosition[0] * Kp_pha);
	man1_yw0 = man1_yw - (-pPosition[2] * Kp_pha);
	man1_zw0 = man1_zw - (pPosition[1] * Kp_pha_z);

	hDevice_move_r = hDevice_RU;
	move_flag = 2;
	move_mode = 2;
}

void pipette_set_b() {
	double man3_set[3] = { 3650, -2850, -2200 };
	double man13_store[3] = { 0,0,-2250 };
	if (move_mode == 2) Getposition(hDevice_RU, man13_store[0], man13_store[1], man13_store[2]);
	else if (move_mode == 1) Getposition(hDevice_RD, man13_store[0], man13_store[1], man13_store[2]);
	sleep(20);
	phase_flag = 2;
	move_flag = 0;
	move_mode = 0;
	//移動
	sleep(100);
	std::vector<double> movepos3{ man13_store[0] + 300 ,man13_store[1] - 350 ,man3_set[2] }; //待機ポジ、後で決める
	Movepos(hDevice_RD, s, movepos3);
	man3_xw = man13_store[0] + 300;
	man3_yw = man13_store[1] - 350;
	man3_zw = man3_set[2];
	sleep(200);
	std::vector<double> movepos1{ man13_store[0],man13_store[1],-2240 + man1_auto_dz }; //待機ポジ、後で決める
	Movepos(hDevice_RU, s, movepos1);
	man1_xw = man13_store[0];
	man1_yw = man13_store[1];
	man1_zw = -2250 + man1_auto_dz;
	man1_xw0 = man1_xw - (pPosition[0] * Kp_pha);
	man1_yw0 = man1_yw - (-pPosition[2] * Kp_pha);
	man1_zw0 = man1_zw - (pPosition[1] * Kp_pha_z);
	hDevice_move_r = hDevice_RU;
	sleep(100);
	move_flag = 2;
	move_mode = 2;
}

void Move_Pipette_Getposition(double man13_store[3]) {
	if (move_mode == 2) Getposition(hDevice_RU, man13_store[0], man13_store[1], man13_store[2]);
	else if (move_mode == 1) Getposition(hDevice_RD, man13_store[0], man13_store[1], man13_store[2]);
}

HDCallbackCode HDCALLBACK DevicePositionCallback(void* pUserData)
{
	HDdouble* pPosition = (HDdouble*)pUserData;

	hdBeginFrame(hdGetCurrentDevice());
	hdGetDoublev(HD_CURRENT_POSITION, pPosition);
	hdEndFrame(hdGetCurrentDevice());

	HDErrorInfo error;
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Error during main scheduler callback\n");

		if (hduIsSchedulerError(&error))
		{
			return HD_CALLBACK_DONE;
		}
	}

	return HD_CALLBACK_DONE;
}

//視野拡張時の視点から操作者によって狭められた視野範囲内の視点のみ抽出しplist_angleに入れる
int make_angle_table2() {
	double MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;
	int center_framex2 = center_framex;
	int center_framey2 = center_framey;
	int expand_viewpoint_num = 0;
	double* p = plist[0];

	X2 = 0;

	int min_posx = (center_framex2 + (-1 - MAG) * frame_width / 2 - IMG_WIDTH / 8) * resolution;
	int max_posx = (center_framex2 + (-1 + MAG) * frame_width / 2 + IMG_WIDTH / 8) * resolution;
	int min_posy = (center_framey2 + (-1 - MAG) * frame_height / 2 - IMG_HEIGHT / 8) * resolution;
	int max_posy = (center_framey2 + (-1 + MAG) * frame_height / 2 + IMG_HEIGHT / 8) * resolution;

	//printf("MAG=%f,minx=%d,maxx=%d,miny=%d,maxy=%d\n", MAG, min_posx, max_posx, min_posy, max_posy);

	for (int i = X - 1; i > -1; i--) {
		p = plist[i];
		ndata2[X2] = 0;
		for (int j = 0; j < ndata[i]; j++) {
			if ((*(p + 2) <= max_posx) && (*(p + 2) >= min_posx) && (*(p + 3) <= max_posy) && (*(p + 3) >= min_posy)) {

				plist_angle[expand_viewpoint_num] = p;

				printf("X2=%d,pointer=%p,xangle=%f,yangle=%f,ndata2=%d\n", X2, plist_angle[expand_viewpoint_num], *plist_angle[expand_viewpoint_num], *(plist_angle[expand_viewpoint_num] + 1), ndata2[X2]);
				expand_viewpoint_num++;
				ndata2[X2]++;

			}

			p = p + 4;

		}
		if (ndata2[X2] > 0) X2++;
	}
	return expand_viewpoint_num;
}

std::string error_message;
//画像が正常かチェック
bool validateImage(const cv::Mat& img, std::string& error_message) {
	try {
		// 1. 画像が空でないかチェック
		if (img.empty()) {
			error_message = "Input image is empty";
			return false;
		}

		//// 2. メモリ確保チェック
		//if (!img.data) {
		//	error_message = "Failed to allocate memory for the image";
		//	return false;
		//}

		// 3. 画像サイズチェック
		if (img.rows <= 0 || img.cols <= 0) {
			error_message = "Invalid image dimensions: " +
				std::to_string(img.rows) + "x" +
				std::to_string(img.cols);
			return false;
		}

		// 4. 画像サイズの上限チェック
		//const int MAX_IMAGE_SIZE = 10000;  // 適切な上限値に調整してください
		//if (img.rows > MAX_IMAGE_SIZE || img.cols > MAX_IMAGE_SIZE) {
		//	error_message = "Image dimensions exceed maximum allowed size (" +
		//		std::to_string(MAX_IMAGE_SIZE) + ")";
		//	return false;
		//}

		// 5. データ型チェック
		//if (img.depth() != CV_8U && img.depth() != CV_8S &&
		//	img.depth() != CV_16U && img.depth() != CV_16S &&
		//	img.depth() != CV_32F && img.depth() != CV_64F) {
		//	error_message = "Invalid image data type: " +
		//		std::to_string(img.depth());
		//	return false;
		//}

		//// 6. チャンネル数チェック
		//if (img.channels() != 1 && img.channels() != 3 && img.channels() != 4) {
		//	error_message = "Invalid number of channels: " +
		//		std::to_string(img.channels());
		//	return false;
		//}

		//// 7. ピクセル値の範囲チェック（8ビット画像の場合）
		//if (img.depth() == CV_8U) {
		//	double minVal, maxVal;
		//	cv::minMaxLoc(img, &minVal, &maxVal);
		//	if (maxVal > 255 || minVal < 0) {
		//		error_message = "Pixel values out of valid range [0,255]";
		//		return false;
		//	}
		//}

		// 8. データの連続性チェック（警告のみ）
		if (!img.isContinuous()) {
			std::cout << "Warning: Image data is not continuous" << std::endl;
			return false;
		}

		return true;

	}
	catch (const cv::Exception& e) {
		error_message = "OpenCV error: " + std::string(e.what());
		return false;
	}
	catch (const std::exception& e) {
		error_message = "Standard error: " + std::string(e.what());
		return false;
	}
	catch (...) {
		error_message = "Unknown error occurred";
		return false;
	}
}


int max_expand_viewpoint_num = 0;
int expand_viewpoint_num = 0;
int countX = 0;
int count_ndata = 0;
int count_Xndata = 0;
int npointed_count2 = 0;
char window_name[] = "Plane_image";
char pointed_window_name[20];
int t_REC_switch = 0;
int REC_switch = 0;
//#define max_save_count  100
int delay_frame = 1;
int delay_pointedframe = 0;
int store_image_count = delay_frame;
int store_angle_count = delay_frame;

int* store_img_cnt;
int store_angle_cnt = 0;
int* cnt_Xndata;
int cnt_ndata = 0;

//int store_image_count2 = 0;
int store_pointed_image_count = 0;
int count_nstoreimage = 0;
//int count_nstoreangle = 0;
bool isClick = true;
//int frame_height=(l3)*1000*dtan(*(plist[X]+ndata[X]/4))+IMG_HEIGHT;
int t_imagenframe = 0;
int thread_start_num = 0;
int s_expand_viewpoint_num[1024][100];
double s_MAG[1024][100];
double s_fps[1024][100];
double s_time_frame[1024][100];
int s_count1 = 0;
int s_count2 = 0;
int sc_count2[1024];

int s_pointed_count[1024][100];
double s_t2fps[1024][100];
double s_t2time_frame[1024][100];
int s_t2count1 = 0;
int s_t2count2 = 0;
int sc_t2count2[1024];

//Mat pointed_frame(IMG_HEIGHT2, IMG_WIDTH2, CV_8UC1);

HANDLE hEvent[12];
HANDLE hTh[12];
HANDLE hMtx;	//ミューテックス用ハンドル@try
HANDLE mutexh;
int end_flag = 0;
int switch_flag = 1;//視野拡張と通常視野の切り替え 視野拡張:1 通常視野:2
int inside_flag = 0;//0：なし 1：円の中に入る瞬間を検知 2：円の内側に入っている
double inside_count = 0;

LARGE_INTEGER qpc0;

//焦点変更画像保存########################
int save_img_depth = 0;
bool save_img_depth_flag = FALSE;
int save_img_depth_range = 5;	//焦点距離を変化させながら保存する枚数（×２＋１が総数）
char save_img_depth_dir[] = "sample_img\\depth\\";
//焦点変更画像保存########################


struct stat statDirectory;

/// <summary>
/// 「XI_IMG型」の画像情報を「uchar型」へ変換する関数．変換時にガンマ補正を行うことも可能．
///ガンマ補正のON/OFF切替に対応	transport_image()とtransport_image_gamma()を統合したもの（OFF：transport_image，ON:transport_image_gamma） 
///ガンマ補正モードでないときに、拡張画像の画像枚数分の輝度値をそれぞれ取得し、その最大値を導出する。
///ガンマ補正モードのときには、導出した最大の輝度値とその時取得した画像の輝度値からガンマ補正値を導出し、ガンマ補正を行う。
/// </summary>
/// <param name="in_p	：XIMEAから取得した１枚の画像情報"></param>
/// <param name="out_p	：画像情報へのポインタ。拡張画像合成時にこれを参照していると考えられる"></param>
/// 

ofstream transport_log("logging\\transportlog.csv");

void transport_image_multi_phot(Mat in_p, uchar* out_p) {	//* out_pはpimageやppointedimageに使用
	//ガンマ補正なしの状態で行う初期設定に関わる変数の定義
	static int number_of_images = 0;	//拡張画像1周分の枚数まで（ガンマ補正モードでないときに）カウントアップ
	static double max_intensity = 0;	//最大の輝度値を保持（ガンマ補正モードでないときに導出）

	//ガンマ補正モードのとき
	if (window_states.gamma_view_flag == TRUE) {
		double gamma;				//画像に適用するガンマ補正値
		double s_gamma = 1.9;		//ガンマ補正値導出時の補正係数
		Mat frame_mono(IMG_HEIGHT2, IMG_WIDTH2, CV_8UC1);	//モノクロ画像として取得　@cmt:IMG_HEIGHT"2" を使う理由はわからない
		//Mat frame_mono(IMG_HEIGHT, IMG_WIDTH, CV_8UC1);	//モノクロ画像として取得　@cmt:IMG_HEIGHT"2" を使う理由はわからない
		uchar lut[256];
		frame_mono.data = (uchar*)in_p.data;
		Mat original = frame_mono;	//ガンマ補正適用前の画像
		Mat result;					//ガンマ補正を適用した画像（場合によっては補正なし）

		//この関数が呼び出されたときの画像における輝度値の平均を取得
		double intensity_mean = (int)mean(original)[0];

		//最大の輝度値と現在の輝度値を基にガンマ補正の係数を求める
		gamma = 1 + (max_intensity - intensity_mean) / max_intensity * s_gamma;

		//暗い画像に対して行う処理
		if (gamma > 1) {
			double gm = 1.0 / gamma;
			for (int i = 0; i < 256; i++) {
				lut[i] = pow(1.0 * i / 256, gm) * 256;
			}
			LUT(original, Mat(Size(256, 1), CV_8U, lut), result);	//ガンマ補正を行う
		}
		//輝度が最大の画像の場合
		else {
			result = original;	//変化なし
		}
		uchar* in_p2 = (uchar*)result.data;
		//in_p2 = in_p2 + 4 * (IMG_WIDTH2 + 1);
		//in_p2 = in_p2 + 4 * (IMG_WIDTH + 1);
		for (int image_col = 0; image_col < IMG_HEIGHT; image_col++) {
			//if (image_col > 0) in_p2 = in_p2 + IMG_WIDTH2 - IMG_WIDTH;
			//if (image_col > 0) in_p2 = in_p2// + IMG_WIDTH2 - IMG_WIDTH;
			for (int image_row = 0; image_row < IMG_WIDTH; image_row++) {
				*out_p = *in_p2;
				in_p2++;
				out_p++;
			}
		}
		number_of_images = 0;
	}
	//ガンマ補正モードでないとき
	else {//transport_imageだったもの
		//拡張画像を1周するまでそれぞれの画像の輝度値を取得し、ガンマ補正用にその最大値を求める

		//imshow("in_p", in_p);
		//waitKey(1000);
		//sleep(1000);

		if (number_of_images < expand_viewpoint_num) {
			Mat frame_mono(IMG_HEIGHT2, IMG_WIDTH2, CV_8UC1);	//モノクロ画像として取得　@cmt:IMG_HEIGHT"2" を使う理由はわからない
			//imshow("frame_mono", frame_mono);
			//waitKey(1000);
			//sleep(1000);
			frame_mono.data = (uchar*)in_p.data;
			Mat original = frame_mono;
			//imshow("original", original);
			//waitKey(1000);
			//sleep(1000);
			double intensity_mean = mean(original)[0];	//その時の画像における輝度の平均値を保持
			//取得した画像における輝度値がこれまでで最大のとき、輝度の最大値を置き換える
			if (intensity_mean > max_intensity) {
				max_intensity = intensity_mean;
			}
			number_of_images++;	//枚数カウントアップ
		}

		uchar* in_p2 = (uchar*)in_p.data;
		//in_p2 = in_p2 + 4 * (IMG_WIDTH2 + 1);
		for (int image_col = 0; image_col < IMG_HEIGHT; image_col++) {
			//if (image_col > 0) in_p2 = in_p2 + IMG_WIDTH2 - IMG_WIDTH;
			for (int image_row = 0; image_row < IMG_WIDTH; image_row++) {
				*out_p = *in_p2;
				in_p2++;
				out_p++;
			}
		}
	}
	//uchar* in_p2 = (uchar*)in_p.data;
	////in_p2 = in_p2 + 4 * (IMG_WIDTH2 + 1);
	//for (int image_col = 0; image_col < IMG_HEIGHT; image_col++) {
	//	//if (image_col > 0) in_p2 = in_p2 + IMG_WIDTH2 - IMG_WIDTH;
	//	for (int image_row = 0; image_row < IMG_WIDTH-2; image_row++) {
	//		//if (image_row >= IMG_WIDTH - 2) {
	//		//	in_p2++;
	//		//}
	//		*out_p = *in_p2;
	//		in_p2++;
	//		out_p++;
	//	}
	//}

}

//マウス入力用のパラメータ
struct mouseParam {
	int x;
	int y;
	int event;
	int flags;
};

//コールバック関数
void CallBackFunc(int eventType, int x, int y, int flags, void* userdata)
{
	mouseParam* ptr = static_cast<mouseParam*> (userdata);

	ptr->x = x;
	ptr->y = y;
	ptr->event = eventType;
	ptr->flags = flags;
}

//==================== クリック移動（通常視野 circle_img） ====================
// T + 左クリック : 目標点を設定
// Enter          : 操作中のインジェクションピペット（move_mode 2→man1/RU, 1→man3/RD）を目標点へ XY 移動（Z は固定）
// X              : 目標をクリア / 移動中なら停止
mouseParam g_circleMouse = { 0, 0, -1, 0 };		//circle_img の最新のクリック（判定後に event を -1 に戻す）
std::atomic<bool> g_click_target_set{ false };	//目標点が設定されているか
std::atomic<bool> g_click_move_busy{ false };	//クリック移動の実行中
std::atomic<bool> g_click_move_abort{ false };	//移動中止の要求
double g_click_target_px[2] = { 0, 0 };			//目標点（circle_img の画素座標）
const double CLICK_MOVE_SPEED = 300.0;			//移動速度 [um/s]（既定の s=1000 より遅め）
const double CLICK_MOVE_MAX_DIST = 1000.0;		//これより遠い目標は拒否 [um]
const double CLICK_MOVE_TOLERANCE = 1.0;		//到達判定 [um]

//circle_img のマウスコールバック（既存の 1～4＋クリックの先端補正と、T＋クリックの目標設定）
void CircleImgMouseCallback(int eventType, int x, int y, int flags, void* userdata)
{
	if (eventType != cv::EVENT_LBUTTONDOWN) return;	//マウス移動で上書きされないよう、左クリックだけ記録する

	g_circleMouse.x = x;
	g_circleMouse.y = y;
	g_circleMouse.event = eventType;
	g_circleMouse.flags = flags;

	if ((GetAsyncKeyState(0x54) & 0x8000) && !g_click_move_busy) {	//T キー
		g_click_target_px[0] = x;
		g_click_target_px[1] = y;
		g_click_target_set = true;
		printf("[クリック移動] 目標点を設定 (u, v) = (%d, %d)  Enter で移動 / X でクリア\n", x, y);
	}
}

//目標画素 (u, v) へ操作中のインジェクションピペットを XY 移動する（別スレッドで実行）
//mode : 2 → man1(RU)、1 → man3(RD)
void clickMoveExecute(double u, double v, int mode)
{
	const bool is_man1 = (mode == 2);
	HANDLE dev = is_man1 ? hDevice_RU : hDevice_RD;
	const char* name = is_man1 ? "man1(RU)" : "man3(RD)";
	Mat& rvec = is_man1 ? rvec1 : rvec3;
	Mat& tvec = is_man1 ? tvec1 : tvec3;
	const double tip_dx = is_man1 ? man1_dx : man3_dx;
	const double tip_dy = is_man1 ? man1_dy : man3_dy;
	double& xw = is_man1 ? man1_xw : man3_xw;
	double& yw = is_man1 ? man1_yw : man3_yw;
	double& zw = is_man1 ? man1_zw : man3_zw;
	double& xw0 = is_man1 ? man1_xw0 : man3_xw0;
	double& yw0 = is_man1 ? man1_yw0 : man3_yw0;
	double& zw0 = is_man1 ? man1_zw0 : man3_zw0;
	Mat& pos3D = is_man1 ? man1_pos3D : man3_pos3D;

	//Phantom / Xbox からの指令を止める（A キーのシーケンスと同じ手順）
	const int saved_move_mode = move_mode;
	const int saved_move_flag = move_flag;
	move_flag = 0;
	move_mode = 0;
	sleep(50);

	double x0, y0, z0;
	Getposition(dev, x0, y0, z0);

	double xt = 0, yt = 0;
	bool ok = click_move::pixelToWorldFixedZ(u, v, z0, rvec, tvec, K, tip_dx, tip_dy, xt, yt);
	const double dx = xt - x0;
	const double dy = yt - y0;
	const double dist = sqrt(dx * dx + dy * dy);

	if (!ok) {
		printf("[クリック移動] %s: 座標変換に失敗したため移動しません\n", name);
	}
	else {
		const double err = click_move::reprojectError(u, v, xt, yt, z0, rvec, tvec, K, tip_dx, tip_dy);
		printf("[クリック移動] %s: 現在 (%.1f, %.1f, %.1f) -> 目標 (%.1f, %.1f, %.1f) um  移動量 %.1f um  再投影誤差 %.3f px\n",
			name, x0, y0, z0, xt, yt, z0, dist, err);
		if (dist > CLICK_MOVE_MAX_DIST) {
			printf("[クリック移動] 移動量が上限 %.0f um を超えるため移動しません\n", CLICK_MOVE_MAX_DIST);
			ok = false;
		}
	}

	bool aborted = false;
	if (ok && dist >= 0.05) {	//0.05 um = 1 パルス未満は動かさない
		std::vector<double> spd{ CLICK_MOVE_SPEED, CLICK_MOVE_SPEED, CLICK_MOVE_SPEED };
		std::vector<double> amount{ dx, dy, 0.0 };	//Z は動かさない（Moveto は量 0 の軸を停止のままにする）
		Moveto(dev, spd, amount);

		const double timeout_ms = dist / CLICK_MOVE_SPEED * 1000.0 + 2000.0;
		double elapsed = 0;
		bool reached = false;
		while (elapsed < timeout_ms) {
			elapsed += sleep(5);
			double cx_now, cy_now, cz_now;
			Getposition(dev, cx_now, cy_now, cz_now);
			pos3D.at<double>(0, 0) = cx_now;	//緑の先端マーカーを移動中も追従させる
			pos3D.at<double>(0, 1) = -cy_now;
			pos3D.at<double>(0, 2) = cz_now;

			if (g_click_move_abort || (GetAsyncKeyState(0x58) & 0x8000)) {	//X キーで停止
				DriveStop(dev);
				aborted = true;
				printf("[クリック移動] 停止しました\n");
				break;
			}
			const double ex = xt - cx_now;
			const double ey = yt - cy_now;
			if (sqrt(ex * ex + ey * ey) <= CLICK_MOVE_TOLERANCE) {
				reached = true;
				break;
			}
		}
		if (!reached && !aborted) {
			DriveStop(dev);
			printf("[クリック移動] タイムアウトのため停止しました\n");
		}
		sleep(50);
	}

	//最終位置を設定値に反映し、Phantom のオフセットを取り直す（再開時に跳ね戻らないように）
	double xf, yf, zf;
	Getposition(dev, xf, yf, zf);
	xw = xf;
	yw = yf;
	zw = zf;
	pos3D.at<double>(0, 0) = xf;
	pos3D.at<double>(0, 1) = -yf;
	pos3D.at<double>(0, 2) = zf;
	xw0 = xw - (pPosition2[0] * Kp_pha);
	yw0 = yw - (-pPosition2[2] * Kp_pha);
	zw0 = zw - (pPosition2[1] * Kp_pha_z);
	if (ok && !aborted) {
		printf("[クリック移動] %s: 到達 (%.1f, %.1f, %.1f) um  残差 %.1f um\n", name, xf, yf, zf, sqrt((xt - xf) * (xt - xf) + (yt - yf) * (yt - yf)));
	}

	move_mode = saved_move_mode;
	move_flag = saved_move_flag;
	g_click_target_set = false;
	g_click_move_abort = false;
	g_click_move_busy = false;
}

double EdgeCaluculation(Mat src, int center_x, int center_y, int radius)
{
	int row = 0;
	int col = 0;
	double f = 0.0;
	double f_sum = 0.0;
	double f_ave;
	int x0, y0;	//エッジ強度評価範囲の左上座標
	int step = 2;

	x0 = center_x - radius;
	y0 = center_y - radius;

	for (int i = 0; i < radius * 2 + 1; i++) {
		for (int j = 0; j < radius * 2 + 1; j++) {
			if (y0 + i + 3 < IMG_HEIGHT2 && x0 + j + 3 < IMG_WIDTH2) {
				//row = src.data[IMG_WIDTH2 * (y0 + i) + x0 + j] + src.data[IMG_WIDTH2 * (y0 + i + 1) + x0 + j] - src.data[IMG_WIDTH2 * (y0 + i) + x0 + j + 2] - src.data[IMG_WIDTH2 * (y0 + i + 1) + x0 + j + 2];
				//col = src.data[IMG_WIDTH2 * (y0 + i) + x0 + j] + src.data[IMG_WIDTH2 * (y0 + i) + x0 + j + 1] - src.data[IMG_WIDTH2 * (y0 + i + 2) + x0 + j] - src.data[IMG_WIDTH2 * (y0 + i + 2) + x0 + j + 1];
				row = src.data[IMG_WIDTH2 * (y0 + i) + x0 + j] + src.data[IMG_WIDTH2 * (y0 + i + 1) + x0 + j] + src.data[IMG_WIDTH2 * (y0 + i + 2) + x0 + j] - src.data[IMG_WIDTH2 * (y0 + i) + x0 + j + 3] - src.data[IMG_WIDTH2 * (y0 + i + 1) + x0 + j + 3] - src.data[IMG_WIDTH2 * (y0 + i + 2) + x0 + j + 3];
				col = src.data[IMG_WIDTH2 * (y0 + i) + x0 + j] + src.data[IMG_WIDTH2 * (y0 + i) + x0 + j + 1] + src.data[IMG_WIDTH2 * (y0 + i) + x0 + j + 2] - src.data[IMG_WIDTH2 * (y0 + i + 3) + x0 + j] - src.data[IMG_WIDTH2 * (y0 + i + 3) + x0 + j + 1] - src.data[IMG_WIDTH2 * (y0 + i + 3) + x0 + j + 2];
				//row = src.data[IMG_WIDTH2 * (y0 + i) + x0 + j] + src.data[IMG_WIDTH2 * (y0 + i + 1) + x0 + j] + src.data[IMG_WIDTH2 * (y0 + i + 2) + x0 + j] + src.data[IMG_WIDTH2 * (y0 + i + 3) + x0 + j] + src.data[IMG_WIDTH2 * (y0 + i + 4) + x0 + j] - src.data[IMG_WIDTH2 * (y0 + i) + x0 + j + 5] - src.data[IMG_WIDTH2 * (y0 + i + 1) + x0 + j + 5] - src.data[IMG_WIDTH2 * (y0 + i + 2) + x0 + j + 5] - src.data[IMG_WIDTH2 * (y0 + i + 3) + x0 + j + 5] - src.data[IMG_WIDTH2 * (y0 + i + 4) + x0 + j + 5];
				//col = src.data[IMG_WIDTH2 * (y0 + i) + x0 + j] + src.data[IMG_WIDTH2 * (y0 + i) + x0 + j + 1] + src.data[IMG_WIDTH2 * (y0 + i) + x0 + j + 2] + src.data[IMG_WIDTH2 * (y0 + i) + x0 + j + 3] + src.data[IMG_WIDTH2 * (y0 + i) + x0 + j + 4] - src.data[IMG_WIDTH2 * (y0 + i + 5) + x0 + j] - src.data[IMG_WIDTH2 * (y0 + i + 5) + x0 + j + 1] - src.data[IMG_WIDTH2 * (y0 + i + 5) + x0 + j + 2] - src.data[IMG_WIDTH2 * (y0 + i + 5) + x0 + j + 3] - src.data[IMG_WIDTH2 * (y0 + i + 5) + x0 + j + 4];
				f = sqrt((row * row + col * col) * 1.0);
				//f = abs(2 * src.data[IMG_WIDTH2 * (y0 + i) + x0 + j] - src.data[IMG_WIDTH2 * (y0 + i) + x0 + j - step] - src.data[IMG_WIDTH2 * (y0 + i) + x0 + j + step]) + abs(2 * src.data[IMG_WIDTH2 * (y0 + i) + x0 + j] - src.data[IMG_WIDTH2 * (y0 + i - step) + x0 + j] - src.data[IMG_WIDTH2 * (y0 + i + 2) + x0 + step]);
				//src.data[IMG_WIDTH2 * (y0 + i) + x0 + j] = 255;
			}
			else {
				f = 0.0;
			}
			f_sum = f_sum + f;
		}
	}
	f_ave = f_sum / ((radius * 2 + 1.0) * (radius * 2 + 1.0));
	return f_ave;
}

void man_detection(Mat view, Mat back, int thres, int RorL, double& x_man, double& y_man) {
	//2値化処理
	Mat threshold_img(view.rows, view.cols, CV_8UC1);
	//threshold(view, threshold_img, thres, 255, THRESH_BINARY_INV);

	for (int y = 0; y < IMG_HEIGHT2; y++) {
		for (int x = 0; x < IMG_WIDTH2; x++) {
			if (abs(view.data[IMG_WIDTH2 * y + x] - back.data[IMG_WIDTH2 * y + x]) > thres) {
				threshold_img.data[IMG_WIDTH2 * y + x] = 255;
			}
			else {
				threshold_img.data[IMG_WIDTH2 * y + x] = 0;
			}
		}
	}

	imshow("back_sub", threshold_img);
	//imwrite("背景差分.png", threshold_img);

	Mat median_img;
	medianBlur(threshold_img, median_img, 1);
	//medianBlur(threshold_img, median_img, 11);
	imshow("median_img", median_img);

	int y_s, y_e;
	y_s = -1;

	//右端の物体を検出
	if (RorL == 0) {
		for (int x = IMG_WIDTH2 - 5; x > 4; x--) {
			for (int y = 4; y < IMG_HEIGHT2 - 4; y++) {
				if (median_img.data[IMG_WIDTH2 * y + x] == 255) {
					if (y_s == -1)	y_s = y;
					y_e = y;
				}
			}

			if (y_s > -1) {
				x_man = (double)x;
				y_man = (y_s + y_e) / 2.0;
				break;
			}
		}
	}

	//左端の物体を検出
	if (RorL == 1) {
		for (int x = 4; x < IMG_WIDTH2 - 4; x++) {
			for (int y = 4; y < IMG_HEIGHT2 - 4; y++) {
				if (median_img.data[IMG_WIDTH2 * y + x] == 255) {
					if (y_s == -1)	y_s = y;
					y_e = y;
				}
			}

			if (y_s > -1) {
				x_man = (double)x;
				y_man = (y_s + y_e) / 2.0;
				break;
			}
		}
	}

}

void get_man_calib_pettern(Mat objectPoints, Mat imagePoints, int num_x, int num_y, int num_z, int dx, int dy, int dz, double dWD, HANDLE hDevice) {

	double dWD_0 = dWD;
	double pos_x, pos_y, pos_z, pos_x2, pos_y2, pos_z2;
	std::vector<double> s{ 7000,7000,7000 };

	Getposition(hDevice, pos_x, pos_y, pos_z);
	sleep(500);

	for (int k = 0; k < num_z; k++) {
		if (k > 0) {
			//焦点距離変化
			dWD = dWD + dz / 1000.0;
			calculate_etl2(dWD);
			if (current > max_current) current = max_current;
			else if (current < 0) current = 0;
			send_currentset_command(current);

			sleep(50);
		}

		std::vector<double> p{ pos_x, pos_y - 300, pos_z + dz * k };
		Movepos(hDevice, s, p);
		sleep(500);

		////背景差分に用いる背景画像の取得
		Mat back;
		Mat back2;
		cap >> back;
		//flip(back, back, 0);
		back2 = back.clone();

		//resize(back, back, Size(IMG_WIDTH2, IMG_HEIGHT2));
		//Mat back3 = back.clone();

		for (int j = 0; j < num_y; j++) {
			for (int i = 0; i < num_x; i++) {

				//sprintf(command, "12 %d %d %d %d %d %d", (int)pos_x + dx * i, (int)pos_y + dy * j, (int)pos_z + dz * k, 7000, 7000, 7000);
				//send_man_operate_command(command);
				//sleep(200);
				//recieve_man_command();
				std::vector<double> p{ pos_x + dx * i, pos_y + dy * j, pos_z + dz * k };
				Movepos(hDevice, s, p);
				sleep(500);

				//Mat view(IMG_HEIGHT2, IMG_WIDTH2, CV_8UC1);
				//view.data = (uchar*)image.bp;
				//flip(view, view, 0);

				Mat view;
				Mat view2;
				flip(view, view, 0);
				cap >> view;
				view2 = view.clone();
				//resize(view, view, Size(IMG_WIDTH2, IMG_HEIGHT2));

				objectPoints.at<double>(num_x * num_y * k + num_x * j + i, 0) = pos_x + dx * i;
				objectPoints.at<double>(num_x * num_y * k + num_x * j + i, 1) = -(pos_y + dy * j);//カメラ座標系にy軸の向きを合わせるためマイナスをつける
				objectPoints.at<double>(num_x * num_y * k + num_x * j + i, 2) = pos_z + dz * k;

				imshow("back_view", back2);
				imshow("view_view", view2);

				double img_x, img_y;
				man_detection(view2, back2, Threshold, 1, img_x, img_y);

				imagePoints.at<double>(num_x * num_y * k + num_x * j + i, 0) = img_x;
				imagePoints.at<double>(num_x * num_y * k + num_x * j + i, 1) = img_y;
				cout << "i =  " << i << "  j = " << j << "  img_x = " << img_x << "   " << "img_y = " << img_y << endl;

				cv::circle(view2, cv::Point((int)img_x, (int)img_y), 2, cv::Scalar(255, 255, 255), 5);
				imshow("calib_view", view2);
				waitKey(1);

			}
			printf("loopA");
		}
		printf("loopB");
	}

	//焦点距離変化
	dWD = dWD_0;
	calculate_etl2(dWD);
	if (current > max_current) current = max_current;
	else if (current < 0) current = 0;
	send_currentset_command(current);

	sleep(1000);

	destroyWindow("calib_view");
	destroyWindow("view_view");
	destroyWindow("back_view");
	destroyWindow("back_sub");
	destroyWindow("median_img");
	//destroyAllWindows();
	waitKey(1);
	printf("calib_end");
}

//カメラ座標をマニピュレータ座標に変換(右上のピペット座標)
void camera_to_world2(double CameraPoints[3], double(&WorldPoints)[3]) {
	double temp_CameraPoints[3];

	temp_CameraPoints[2] = CameraPoints[2] * pixelpitch / magnification + man4_zc0 - (man4_s_dWD2 - s_dWD_0) * 1000;
	temp_CameraPoints[0] = (CameraPoints[0] - cx - man4_dx) * temp_CameraPoints[2] / f_k;
	temp_CameraPoints[1] = (CameraPoints[1] - cx - man4_dx) * temp_CameraPoints[2] / f_k;

	WorldPoints[0] = rmat4.at<double>(0, 0) * (temp_CameraPoints[0] - tvec4.at<double>(0, 0)) + rmat4.at<double>(0, 1) * (temp_CameraPoints[1] - tvec4.at<double>(1, 0)) + rmat4.at<double>(0, 2) * (temp_CameraPoints[2] - tvec4.at<double>(2, 0)) + CtoW_man4_dx;
	WorldPoints[1] = rmat4.at<double>(1, 0) * (temp_CameraPoints[0] - tvec4.at<double>(0, 0)) + rmat4.at<double>(1, 1) * (temp_CameraPoints[1] - tvec4.at<double>(1, 0)) + rmat4.at<double>(1, 2) * (temp_CameraPoints[2] - tvec4.at<double>(2, 0)) + CtoW_man4_dy;
	WorldPoints[2] = rmat4.at<double>(2, 0) * (temp_CameraPoints[0] - tvec4.at<double>(0, 0)) + rmat4.at<double>(2, 1) * (temp_CameraPoints[1] - tvec4.at<double>(1, 0)) + rmat4.at<double>(2, 2) * (temp_CameraPoints[2] - tvec4.at<double>(2, 0)) + CtoW_man4_dz;
}

//カメラ座標をマニピュレータ座標に変換(左上のピペット座標)
void camera_to_world1(double CameraPoints[3], double(&WorldPoints)[3]) {
	double temp_CameraPoints[3];

	temp_CameraPoints[2] = CameraPoints[2] * pixelpitch / magnification + man2_zc0 - (man2_s_dWD2 - s_dWD_0) * 1000;
	temp_CameraPoints[0] = (CameraPoints[0] - cx - man2_dx) * temp_CameraPoints[2] / f_k;
	temp_CameraPoints[1] = (CameraPoints[1] - cx - man2_dx) * temp_CameraPoints[2] / f_k;

	WorldPoints[0] = rmat2.at<double>(0, 0) * (temp_CameraPoints[0] - tvec2.at<double>(0, 0)) + rmat2.at<double>(0, 1) * (temp_CameraPoints[1] - tvec2.at<double>(1, 0)) + rmat2.at<double>(0, 2) * (temp_CameraPoints[2] - tvec2.at<double>(2, 0)) + CtoW_man2_dx;
	WorldPoints[1] = rmat2.at<double>(1, 0) * (temp_CameraPoints[0] - tvec2.at<double>(0, 0)) + rmat2.at<double>(1, 1) * (temp_CameraPoints[1] - tvec2.at<double>(1, 0)) + rmat2.at<double>(1, 2) * (temp_CameraPoints[2] - tvec2.at<double>(2, 0)) + CtoW_man2_dy;
	WorldPoints[2] = rmat2.at<double>(2, 0) * (temp_CameraPoints[0] - tvec2.at<double>(0, 0)) + rmat2.at<double>(2, 1) * (temp_CameraPoints[1] - tvec2.at<double>(1, 0)) + rmat2.at<double>(2, 2) * (temp_CameraPoints[2] - tvec2.at<double>(2, 0)) + CtoW_man2_dz;
}

//円検出結果から左上マニピュレータ座標へ変換
void circle_to_world1(double(&WorldPoints)[3]) {
	double CirclePoints_ex[3];
	double CirclePoints[3];
	if (p_flag == 0) {
		CirclePoints_ex[0] = left_circle_x; //ビーズor胚
		CirclePoints_ex[1] = left_circle_y;
	}
	else if (p_flag == 1) {
		CirclePoints_ex[0] = t_2Dpos_ex[0]; //ビーズなしで動作確認
		CirclePoints_ex[1] = t_2Dpos_ex[1];
	}


	CirclePoints[0] = ex_to_normalx(CirclePoints_ex[0]);
	CirclePoints[1] = ex_to_normaly(CirclePoints_ex[1]);
	CirclePoints[2] = man2_cpos[2];
	sleep(30);
	camera_to_world1(CirclePoints, WorldPoints);
	sleep(50);
	//std::cout << "left_circle_x1=" << left_circle_x << std::endl;
	//std::cout << "left_circle_y1=" << left_circle_y << std::endl;
}

//円検出結果から左下マニピュレータ座標へ変換
void circle_to_world2(double(&WorldPoints)[3]) {
	double CirclePoints_ex[3];
	double CirclePoints[3];
	if (p_flag == 0) {
		CirclePoints_ex[0] = left_circle_x; //ビーズor胚
		CirclePoints_ex[1] = left_circle_y;
	}
	else if (p_flag == 1) {
		CirclePoints_ex[0] = t_2Dpos_ex[0]; //ビーズなしで動作確認
		CirclePoints_ex[1] = t_2Dpos_ex[1];
	}
	CirclePoints[0] = ex_to_normalx(CirclePoints_ex[0]);
	CirclePoints[1] = ex_to_normaly(CirclePoints_ex[1]);
	CirclePoints[2] = man4_cpos[2];
	sleep(30);
	camera_to_world2(CirclePoints, WorldPoints);
	sleep(50);
	//std::cout << "left_circle_x1=" << left_circle_x << std::endl;
	//std::cout << "left_circle_y1=" << left_circle_y << std::endl;
}


//左上のピペットが卵子を吸引できているか
void check_inj1(bool& hold_flag) {
	hold_flag = false;
	Mat check_frame;
	Mat th_check_frame;
	Mat LabelImg;
	Mat stats;
	Mat centroids;
	check_frame = Mat(t_frame, Rect(int(man2_cpos_ex[0] + 5), int(man2_cpos_ex[1]) - 25, 50, 50)); //ピペット付近のみ切り抜き
	//円検出
	threshold(check_frame, th_check_frame, Threshold, 255, THRESH_BINARY_INV);
	//ラベリング処理
	int nLab = cv::connectedComponentsWithStats(th_check_frame, LabelImg, stats, centroids);
	int NOC = 0;
	for (int i = 1; i < nLab; ++i) {
		int* param = stats.ptr<int>(i);
		int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
		int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
		cout << "width=" << width << endl;
		cout << "height=" << height << endl;

		if (height < dia_max_ex && height > dia_min_check && width < dia_max_ex && width > dia_min_check) {
			int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			center_x[NOC] = x + width / 2;
			center_y[NOC] = y + height / 2;
			radius[NOC] = (width + height) / 4.0;
			cv::circle(t_frame, cv::Point(center_x[NOC], center_y[NOC]), (int)radius[NOC], cv::Scalar(0, 0, 255), 2);
			NOC = NOC + 1;
		}
		if (NOC == LIMIT) break;

		//std::cout << "NOC=" << NOC << std::endl;
	}
	imwrite("check1.jpg", check_frame);
	if (NOC > 0) hold_flag = 1;
}

//左下のピペットが卵子を吸引できているか
void check_inj2(bool& hold_flag) {
	hold_flag = false;
	Mat check_frame;
	Mat th_check_frame;
	Mat LabelImg;
	Mat stats;
	Mat centroids;
	check_frame = Mat(t_frame, Rect(int(man4_cpos_ex[0] + 5), int(man4_cpos_ex[1] - 25), 50, 50)); //ピペット付近のみ切り抜き

	//円検出
	threshold(check_frame, th_check_frame, Threshold, 255, THRESH_BINARY_INV);
	//ラベリング処理
	int nLab = cv::connectedComponentsWithStats(th_check_frame, LabelImg, stats, centroids);
	int NOC = 0;
	for (int i = 1; i < nLab; ++i) {
		int* param = stats.ptr<int>(i);
		int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
		int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
		cout << "width=" << width << endl;
		cout << "height=" << height << endl;

		if (height < dia_max_ex && height > dia_min_check && width < dia_max_ex && width > dia_min_check) {
			int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			center_x[NOC] = x + width / 2;
			center_y[NOC] = y + height / 2;
			radius[NOC] = (width + height) / 4.0;
			cv::circle(t_frame, cv::Point(center_x[NOC], center_y[NOC]), (int)radius[NOC], cv::Scalar(0, 0, 255), 2);
			//cv::circle(check_frame, cv::Point(center_x[NOC], center_y[NOC]), (int)radius[NOC], cv::Scalar(0, 0, 255), 2);
			NOC = NOC + 1;
		}
		if (NOC == LIMIT) break;

		//std::cout << "NOC=" << NOC << std::endl;
	}
	imwrite("check2.jpg", check_frame);
	if (NOC > 0) hold_flag = 1;
}

//特定のピペットが吸引している細胞のサイズを取得
void Get_circle_size(int& C_radius, double& C_center_x, double& C_center_y, double cpos_ex[3]) {
	Mat check_frame;
	Mat th_check_frame;
	Mat LabelImg;
	Mat stats;
	Mat centroids;
	int threshold_limit;

	//int dia_max_ex;
	//int dia_min_ex;

	//二値化
	threshold(t_frame, check_frame, Threshold2, 255, THRESH_BINARY_INV);
	//ピペット先端までを隠す
	threshold_limit = (int)cpos_ex[0];
	for (int y = 0; y < 900; y++) {
		for (int x = 0; x < threshold_limit; x++) {
			check_frame.data[IMG_WIDTH * y + x] = 0;
		}
	}

	th_check_frame = Mat(check_frame, Rect(int(cpos_ex[0] - 10), int(cpos_ex[1] - 40), 80, 80)); //ピペット付近のみ切り抜き

	//ラベリング処理
	int nLab = cv::connectedComponentsWithStats(th_check_frame, LabelImg, stats, centroids);
	int NOC = 0;
	for (int i = 1; i < nLab; ++i) {
		int* param = stats.ptr<int>(i);
		int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
		int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
		cout << "width=" << width << endl;
		cout << "height=" << height << endl;

		if (height < dia_max_ex && height > dia_min_ex && width < dia_max_ex && width > dia_min_ex) {
			int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			center_x[NOC] = x + width / 2;
			center_y[NOC] = y + height / 2;
			radius[NOC] = (width + height) / 4.0;
			//cv::circle(check_frame, cv::Point(center_x[NOC], center_y[NOC]), (int)radius[NOC], cv::Scalar(0, 0, 255), 2);
			NOC = NOC + 1;
		}
		if (NOC == LIMIT) break;

		//std::cout << "NOC=" << NOC << std::endl;
	}

	int max_radius_number = 0;

	cout << "NOC=" << NOC << endl;

	if (NOC > 0) {
		for (int i = 0; i < NOC; ++i) {
			if (radius[max_radius_number] < radius[i]) {
				max_radius_number = i;
			}
			cout << "radius" << i << "=" << radius[i] << endl;
		}
		C_radius = radius[max_radius_number];
		C_center_x = center_x[max_radius_number] + cpos_ex[0] - 10;
		C_center_y = center_y[max_radius_number] + cpos_ex[1] - 40;
	}
	else {
		C_radius = 18;
		C_center_x = cpos_ex[0] + 18;
		C_center_y = cpos_ex[1];
	}

	C_radius = 18;

	imwrite("inside.png", th_check_frame);


}

constexpr int64_t numChannels = 3;
//constexpr int64_t width = 256;
constexpr int64_t width = 80;
constexpr int64_t trimmed_width = 400;
constexpr int64_t trimmed_height = 400;
//constexpr int64_t height = 256;
constexpr int64_t height = 80;
constexpr int64_t numClasses = 103;
constexpr int64_t numInputElements = numChannels * height * width;
constexpr int64_t numOutputElements = height * width * numClasses;
int centerX = 0;
int centerY = 0;
int centerX2 = (centerX * trimmed_width / 648) - width / 2;
int centerY2 = (centerY * trimmed_width / 488) - height / 2;
int moX = 0;
int moY = 0;
int nonzero2;
float radius_2;
float uncertainty;
int trimmed_area;
int circle_area;
Mat detect_image;
cv::Mat mask2 = cv::Mat::zeros(648, 488, CV_8UC1); // マスク画像

//入力画像を前処理する関数
std::vector<float> loadImage(const cv::Mat& view2)
{
	auto image = view2.clone();
	//auto image2 = Mat(image, Rect(18, 18, 64, 64));
	//cout << "centerX" << centerX << endl;
	//cout << "centerY" << centerY << endl;
	/*int centerX2 = (centerX * trimmed_width / 648) - width / 2;
	int centerY2 = (centerY * trimmed_height / 488) - height / 2;
	if (centerX2 < 0) {
		centerX2 = 0;
	}
	if (centerX2 > width) {
		centerX2 = width;
	}
	if (centerY2 < 0) {
		centerY2 = 0;
	}
	if (centerY2 > height) {
		centerY2 = height;
	}*/
	// OpenCVはBGRの順で読み込むのでRGBに変換

	cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

	//centerX = man24_cpos_c[0] - 5;
	//centerY = man24_cpos_c[1] - 200;

	//if (centerX < 0) {
	//	centerX = 0;
	//}
	//if (centerX + trimmed_width > image.cols) {
	//	centerX = image.cols - trimmed_width;
	//}
	//if (centerY < 0) {
	//	centerY = 0;
	//}
	//if (centerY + trimmed_width > image.rows) {
	//	centerY = image.rows - trimmed_width;
	//}

	centerX = 0;
	centerY = 0;

	//flip(image, image, 0);
	cout << "man2_cpos_c=" << centerX << "," << centerY << std::endl;
	//image = Mat(image, Rect(centerX, centerY, trimmed_width, trimmed_height)).clone();

	imwrite("resize1.png", image);
	cv::Mat image2;

	////cv::resize(image, image2, cv::Size(), 80 / image.cols, 80 / image.rows);
	//cv::resize(image, image2, cv::Size(), 0.2, 0.2);
	cv::resize(image, image2, cv::Size(), 80.0 / image.cols, 80.0 / image.rows);
	imwrite("resize2.png", image2);

	//imshow("trimmed_img", image2);
	/*imshow("trimmed_image", image2);
	waitKey(0);*/
	//cout << image << std::endl;
	cout << image.size() << std::endl;
	cout << image2.size() << std::endl;
	//imshow("image", image);

	// 1次元に変更
	image2 = image2.reshape(1, 1);

	// [0, 255]のuint_8から[0, 1]のfloatに変換
	std::vector<float> vec;
	image2.convertTo(vec, CV_32FC1, 1. / 255);

	// HWC -> CHW
	std::vector<float> output;
	for (size_t ch = 0; ch < 3; ++ch) {
		for (size_t i = ch; i < vec.size(); i += 3) {
			output.emplace_back(vec[i]);
		}
	}
	return output;
}

void initialize_parameters() {
	//MAG_W = 1.0;		//値の初期化
	//MAG_level = 1;		//値の初期化
	//tmp_MAG_level = 1;	//値の初期化
	//start_mirror = 0;
	end_flag = 0;
	//window_states.switch_window_state = 0;
}

void transparency() {
	//モデルの入力
	const std::wstring modelFile = L"train_opset17_117_4to103_add_size80_4layer_005.onnx";

	Ort::Env env;
	Ort::SessionOptions sessionOptions;
	sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_DISABLE_ALL);
	Ort::Session session(nullptr);

	try {
		session = Ort::Session(env, modelFile.data(), sessionOptions);
	}
	catch (Ort::Exception& e) {
		std::cout << e.what() << std::endl;
		return;
	}

	// 入出力のshape
	const std::vector<int64_t> inputShape = { 1, numChannels, height, width };
	const std::vector<int64_t> outputShape = { 1, numClasses, height, width };

	// 入出力用の配列
	std::vector<float> input(numInputElements);
	std::vector<float> results1(numOutputElements);
	std::vector<float> results2(numOutputElements);
	std::vector<float> results3(numOutputElements);
	std::vector<float> results4(numOutputElements);
	std::vector<float> results5(numOutputElements);
	std::vector<float> results6(numOutputElements);
	std::vector<float> results7(numOutputElements);
	std::vector<float> results8(numOutputElements);
	std::vector<float> results9(numOutputElements);
	std::vector<float> results10(numOutputElements);

	// 入出力用のTensor
	// 内部で配列の先頭ポインタを保持する 
	//ここでとまってる
	auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
	auto inputTensor = Ort::Value::CreateTensor<float>(memory_info, input.data(), input.size(), inputShape.data(), inputShape.size());
	auto outputTensor1 = Ort::Value::CreateTensor<float>(memory_info, results1.data(), results1.size(), outputShape.data(), outputShape.size());
	//auto outputTensor2 = Ort::Value::CreateTensor<float>(memory_info, results2.data(), results2.size(), outputShape.data(), outputShape.size());
	//auto outputTensor3 = Ort::Value::CreateTensor<float>(memory_info, results3.data(), results3.size(), outputShape.data(), outputShape.size());
	//auto outputTensor4 = Ort::Value::CreateTensor<float>(memory_info, results4.data(), results4.size(), outputShape.data(), outputShape.size());
	//auto outputTensor5 = Ort::Value::CreateTensor<float>(memory_info, results5.data(), results5.size(), outputShape.data(), outputShape.size());
	//auto outputTensor6 = Ort::Value::CreateTensor<float>(memory_info, results6.data(), results6.size(), outputShape.data(), outputShape.size());
	//auto outputTensor7 = Ort::Value::CreateTensor<float>(memory_info, results7.data(), results7.size(), outputShape.data(), outputShape.size());
	//auto outputTensor8 = Ort::Value::CreateTensor<float>(memory_info, results8.data(), results8.size(), outputShape.data(), outputShape.size());
	//auto outputTensor9 = Ort::Value::CreateTensor<float>(memory_info, results9.data(), results9.size(), outputShape.data(), outputShape.size());
	//auto outputTensor10 = Ort::Value::CreateTensor<float>(memory_info, results10.data(), results10.size(), outputShape.data(), outputShape.size());

	////入力画像取得（画像版）
	//const std::string imageFile = "movieframe_1114.png";
	//const cv::Mat imageFile = "movieframe_1114.png";
	cout << "Get_image_flag =" << get_image_flag << endl;
	cv::Mat imageMat = small_img.clone();

	if (!validateImage(imageMat, error_message)) {
		std::cout << "image validation failed" << endl;
		return;
	}

	//imwrite("movieframe_1114_2.png", small_img);
	//imshow("small_img2", small_img);
	imwrite("small_img.png", small_img);
	imwrite("movieframe.png", imageMat);
	waitKey(1);
	//cv::Mat imageMat = cv::imread(imageFile, cv::IMREAD_COLOR);

	const std::vector<float> imageVec = loadImage(imageMat); // CHW形式を1次元にした画像データ
	assert(imageVec.size() == numInputElements);

	//入力画像取得（動画版）
	//const std::vector<float> imageVec = loadImage(t_frame); // CHW形式を1次元にした画像データ
	//std::cout << "start6666666666666666666666666666666666666666666666666666666666666666" << std::endl;
	//assert(imageVec.size() == numInputElements); //サイズがあっているか確認

	// 画像データを入力用配列にコピー
	std::copy(imageVec.begin(), imageVec.end(), input.begin()); //imageVecの全部をinputに頭からコピー


	// 入出力の名前
// ONNXモデルの保存時と同じ名前にすること
	const std::array<const char*, 1> inputNames = { "input0" };
	const std::array<const char*, 1> outputNames = { "output0" };

	std::vector<int> vector1;

	//推論実行
	Ort::RunOptions runOptions;
	session.Run(runOptions, inputNames.data(), &inputTensor, 1, outputNames.data(), &outputTensor1, 1);

	//推論結果の処理1
	cout << "koko1" << std::endl;
	torch::Tensor tsr1 = torch::tensor(torch::ArrayRef<float>(results1));
	tsr1 = tsr1.reshape({ 103, width, height });
	auto softmax1 = tsr1.softmax(0);
	auto pred1 = softmax1.argmax(0);
	auto one_hot1 = torch::nn::functional::one_hot(pred1);
	one_hot1 = one_hot1.permute({ 2, 1, 0 });

	auto out_tensor1 = one_hot1[101].mul(255).clamp(0, 255).to(torch::kU8);
	cv::Mat mat1(width, height, CV_8U);
	std::memcpy((void*)mat1.data, out_tensor1.data_ptr(), sizeof(torch::kU8) * out_tensor1.numel());
	cv::Mat newmat1 = mat1 * 255;
	//mat1 = mat1.reshape(0, 1);
	//mat1.copyTo(vector1);

	//imshow("newmat1", newmat1);
	imwrite("newmat1.png", newmat1);
	waitKey(1);

	//torch::Tensor result = torch::tensor(torch::ArrayRef<int>(vector1));
	//result = result.reshape({ 3, width, height });

	//auto mode = torch::mode(result, 0);
	//auto mode_tensor = get<0>(mode).to(torch::kU8);
	//cout << "mode_tensor" << mode_tensor << std::endl;
	cv::Mat mode_mat(80, 80, CV_8UC1);
	//std::memcpy((void*)mode_mat.data, mode_tensor.data_ptr(), sizeof(torch::kU8) * mode_tensor.numel());
	//cout << "mode_mat" << mode_mat << std::endl;
	//flip(mode_mat, mode_mat, 0);
	//mode_mat = mode_mat * 255;
	cv::Mat mode_mat2(80, 80, CV_8UC1);

	//最大の領域のみ抽出
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mode_mat, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	auto maxcontour = std::max_element(contours.begin(), contours.end(), [](std::vector<cv::Point> x, std::vector<cv::Point> y) {
		return cv::contourArea(x) < cv::contourArea(y);
		});

	cv::Mat polar_mat = cv::Mat::zeros(mode_mat2.size(), CV_8UC3);
	std::vector<std::vector<cv::Point> > contours2;

	if (contours.empty())
		std::cout << "no polar" << std::endl;
	else
		contours2.push_back(*maxcontour);
	std::cout << "max contours" << contours2 << std::endl;
	cv::drawContours(polar_mat, contours2, -1, cv::Scalar(255, 255, 255), -1);

	cout << "koko2" << std::endl;

	cv::Mat polar_mat2;
	cv::resize(polar_mat, polar_mat2, cv::Size(), 488.0 / polar_mat.cols, 488.0 / polar_mat.rows);
	//imshow("polar_mat", polar_mat2);
	//waitKey(1);

	//cv::resize(mode_mat, mode_mat, cv::Size(), 488.0 / mode_mat.cols, 488.0 / mode_mat.rows);
	//imshow("mat_image", mode_mat);
	//waitKey(1);

	//元画像に円を描画
	Mat Labelmg;
	Mat stats;
	Mat centroids;
	int nLab = connectedComponentsWithStats(mode_mat, Labelmg, stats, centroids);
	int Max = 0;
	int MaxID = 0;

	for (int i = 1; i < nLab; ++i) {//全てのラベルを回す
		int* param = stats.ptr<int>(i);
		if (Max < param[ConnectedComponentsTypes::CC_STAT_AREA]) {//最大値より大きければ
			Max = param[ConnectedComponentsTypes::CC_STAT_AREA];//最大値を更新
			MaxID = i;
			trimmed_area = Max;
		}
	}

	double* mom = centroids.ptr<double>(MaxID);
	//int centerX3 = (centerX * trimmed_width / 644) - width / 2;
	//int centerY3 = (centerY * trimmed_width / 488) - height / 2;
	//if (centerX3 < 0) {
	//	centerX3 = 0;
	//}
	//if (centerX3 > width) {
	//	centerX3 = width;
	//}
	//if (centerY3 < 0) {
	//	centerY3 = 0;
	//}
	//if (centerY3 > height) {
	//	centerY3 = height;
	//}
	int centerX3 = 0;
	int centerY3 = 0;

	/*cout << "centerX" << centerX3 << std::endl;
	cout << "centerY" << centerY3 << std::endl;*/
	moX = static_cast<int>((mom[0] + centerX3) * 648 / trimmed_width);
	moY = 488 - static_cast<int>((mom[1] + centerY3) * 488 / trimmed_width);
	//moY = static_cast<int>((height - mom[1] + centerY3) * 488 / trimmed_width);

	//int roi_x = centerX3, roi_y = centerY3;  // 画像を貼り付ける左上の点
	//int roi_x = centerX, roi_y = man2_cpos_c[1]-160;  // 画像を貼り付ける左上の点

	cout << "mom0" << mom[0] << "," << "mom1" << mom[1] << std::endl;

	float radius;


	circle_area = trimmed_area * (648 * 488) / (width * height);
	radius = static_cast<int>(sqrt(circle_area / M_PI));
	//if (isnan(uncertainty)) {
	//	radius = 20;
	//}
	//else if (uncertainty > 1.0) {
	//	radius = 100;
	//}
	//else if (uncertainty < 0.01) {
	//	radius = 20;
	//}
	//else {
	//	radius = 100 * sqrt(uncertainty);
	//	//radius = 200 *uncertainty;
	//}
	radius_2 = radius;

	//マスク画像作成
	//cv::Mat bg = cv::Mat::zeros(160, 160, CV_8UC1); // 背景画像
	bg = cv::Mat::zeros((int)(IMG_HEIGHT * mag_pointed_frame), (int)(IMG_WIDTH * mag_pointed_frame), CV_8UC1); // 背景画像
	//cv::Mat polar_mat3;

	//polar_mat.convertTo(polar_mat3, CV_8UC1);
	//cvtColor(newmat1, newmat1, cv::COLOR_RGB2GRAY);
	cv::Mat newmat2; // 前景画像
	newmat1.convertTo(newmat2, CV_8UC1);

	//imshow("newmat2", newmat1);
	//imwrite("newmat2.png", newmat1);

	//cv::Mat fg = newmat1; // 前景画像
	////cv::Mat fg;
	//newmat1.copyTo(fg);
	cv::resize(newmat2, newmat2, cv::Size(), bg.cols / newmat2.cols, bg.rows / newmat2.rows);
	cout << newmat1.size() << std::endl;
	cout << newmat2.size() << std::endl;
	cout << bg.size() << std::endl;
	//cout << "roi = " << roi_x << "," << roi_y << endl;

	cv::Mat roi = bg(cv::Rect(centerX, centerY, newmat2.cols, newmat2.rows));
	cout << roi.size() << std::endl;

	//// 前景画像の画素を背景画像にコピー
	//cv::Mat roi = bg(cv::Rect(roi_x, roi_y, fg.cols, fg.rows));
	//fg.copyTo(roi);
	//cv::resize(bg, mask2, cv::Size(), 648.0 / bg.cols, 488.0 / bg.rows);

	// 前景画像の画素を背景画像にコピー
	/*cv::Mat roi = bg(cv::Rect(roi_x, roi_y, fg.cols, fg.rows));*/
	newmat2.copyTo(roi);
	//cv::resize(bg, mask2, cv::Size(), 648.0 / bg.cols, 488.0 / bg.rows);
	//cv::imshow("mask_image", bg);
	//imwrite("roi", roi);
	//cv::waitKey(1);
	//imwrite("newmat2", newmat2);
	//cv::waitKey(1);
	imwrite("mask.png", bg);
	cv::waitKey(1);

	//auto result2 = (result / 255).to(torch::kF32);
	//auto var = at::var(result2, 0);
	////cout << var << std::endl;
	//auto sum = at::sum(var);
	//auto sum2 = sum.item<float>();
	//cout << "sum" << sum2 << std::endl;
	////cout << pred_image << std::endl;
	//auto nonzero = at::count_nonzero(one_hot3[100]);
	//nonzero2 = nonzero.item<float>();
	//cout << "nonzero" << nonzero << std::endl;
	//uncertainty = sum2 / nonzero2;
	//cout << "uncertainty" << uncertainty << std::endl;

	/*if (nonzero2 > 1) {
	cv::circle(view2, cv::Point(moX, moY), radius, cv::Scalar(0, 255, 0), 2, cv::LINE_4);
	}
	else {
		cout << "Polar is not detected." << std::endl;
	}

	imshow("circle_image2", view2);

	waitKey(1);*/
	/*output << view2;*/
	//std::cout << "finish" << std::endl;
	//end = std::chrono::system_clock::now();  // 計測終了時間
	//double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	//std::cout << "time" << elapsed << std::endl;//推論に要した時間(ms)

	int key = cv::waitKey(1);
	//if (key == 'q') {

	//	cv::destroyWindow("circle_image");
	//	video.release();
	//	break; //whileループから抜ける
	//}


}

//マルチスレッド０　ETLの温度情報取得
unsigned __stdcall mythread0(LPVOID lpx)
{
	for (;;) {
		WaitForSingleObject(hEvent[0], INFINITE);

		send_temperaturereading_command();
		recieve_ETL_command();
		printf("Temp=%f\n", ETL_Temp);
		sleep(5000);

		if (end_flag == 1) break;

	}
	return 0;
}

ofstream XimageCreationTime("logging\\multiThXimageCrtTime.csv");
double time_Xcrt1 = 0;
double time_Xcrt2 = 0;
double time_Xcrt3 = 0;
int Xcrt_exp_vp_num[3] = { 0,0,0 };

//マルチスレッド１　視野拡張画像合成と表示
unsigned __stdcall mythread1(LPVOID lpx)
{
	Mat frame2;	//拡張画像
	//Mat frame3(frame_height * IMG_WIDTH * MAG_IMG / frame_width, IMG_WIDTH * MAG_IMG + IMG_WIDTH2, CV_8UC1); //選択した画像
	//namedWindow(window_name);
	////moveWindow(window_name, IMG_WIDTH + 600, 50);
	//moveWindow(window_name, windowframe_xpos - 9, windowframe_ypos - 32);	//初期設定：表示するウィンドウをモニタ中心に移動させる。（常に表示位置を固定したい場合はfor文中に設定する）  MAG_IMGが大きすぎるときの対策するべき

	//setMouseCallback(window_name, mouse_callback, &isClick);
	double MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;
	double t_MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;
	double MAG_thrd_local = MAG;


	int t_store_image_count = 0;
	int t_count_Xndata = 0;
	int t_expand_viewpoint_num = expand_viewpoint_num;

	int pointed_count2 = pointed_count;
	int t_pointed_count2 = pointed_count2;
	Mat frame(frame_height, frame_width, CV_8UC1);
	Mat Depth(frame_height, frame_width, CV_8UC1);
	center_framex = frame_width / 2;
	center_framey = frame_height / 2;
	LARGE_INTEGER frame_qpc1;
	LARGE_INTEGER frame_qpc2;
	double time_frame = 0;
	double t_time_frame = time_frame;
	//int count_update = 0;
	double magnification2 = magnification;
	//int expand_viewpoint_num2 = expand_viewpoint_num;
	//int t_expand_viewpoint_num2 = expand_viewpoint_num2;
	int center_pointedframex = center_framex;
	int center_pointedframey = center_framey;
	double pointed_MAG = MAG;
	int s_flag = 1;


	//QueryPerformanceCounter(&frame_qpc1);
	WaitForSingleObject(hEvent[1], INFINITE);

	for (;;) {
		//printf("スレッド1");
		SuspendThread(hTh[1]);
		if (end_flag == 1) break;
		QueryPerformanceCounter(&frame_qpc1);
		//cout << "th1_start" << endl;

		if (switch_flag == 1)
		{
			//拡張画像合成に用いる画像番号の取得
			ClTest.load_ExpCount_data(t_store_image_count, t_count_Xndata, t_expand_viewpoint_num, MAG_thrd_local);
			//cout << store_image_count2 << "," << count_Xndata2 << endl;
			Xcrt_exp_vp_num[0] = t_expand_viewpoint_num;


			//sleep(100);
			//if (thread_start_num < 15) sleep(300);
			t_pointed_count2 = pointed_count2;
			pointed_count2 = pointed_count;

			//t_expand_viewpoint_num2 = expand_viewpoint_num2;
			//expand_viewpoint_num2 = expand_viewpoint_num;

			Mat Cut_frame;
			frame2 = Scalar(0);

			//printf("diff_cx=%f, diff_cy=%f\n", diff_centerx, diff_centery);
			//printf("t_store=%d,store=%d\n",t_store_image_angle_count,store_image_angle_count2);
			//if (count_nstoreimage > 0 || (count_nstoreimage == 0 && count_Xndata2 > delay_frame)) {
				//printf("angle=%d,image=%d,x=%f,y=%f\n",store_angle_count,store_image_count, *pangle[store_image_count2], *(pangle[store_image_count2]+1));

			double diff_centerx = 0;
			double diff_centery = 0;

			//合成に用いる画像番号がnstoreimage(確保している画像情報のメモリ領域（画像枚数）)をまたぐときと，またがないときで分岐が必要（後者のみでも動作はするが，またいだときに生成されないため画面が一瞬暗転する）
			//nstoreimageをまたがないとき
			if (t_store_image_count >= t_expand_viewpoint_num - 1) {
				//子スレッド側で合成の順番を考慮しなくてもいいように，親スレッド側で調整してある．（親スレッド側：中心の画像を保存したら１周したという判定にし，子スレッドを稼働状態へ変更している）
				for (int count_store_image = t_store_image_count - t_expand_viewpoint_num + 1; count_store_image < t_store_image_count + 1; count_store_image++) {
					//for (int count_store_image = store_image_count2-expand_viewpoint_num+1+8; count_store_image < store_image_count2 - expand_viewpoint_num + 2+8; count_store_image++) {	//一枚だけ配置したいとき（検証用）
						//cout << "A:" << count_store_image << ",s:"<< store_image_count2 - expand_viewpoint_num + 1 << ",e:" << store_image_count2 + 1 <<  endl;
					create_image(z_in, pangle[count_store_image], pimage[count_store_image], frame, Depth, 0., resolution);
					//contrast2(pimage[count_store_image], contrast_pimage);
					//create_image(z_in, pangle[count_store_image - 1], pimage[count_store_image], frame, Depth, 0., resolution);
					//count_update++;
					//printf("%d,mirror_anglex=%f,mirror_angley=%f\n", count_store_image, *pangle[count_store_image], *(pangle[count_store_image]+1));
				}


				ClTest.expanded_CV_rect_setting(MAG_thrd_local, center_framex, center_framey, mouse_posx, mouse_posy);
				ClTest.pointed_rectangle_setting(MAG_thrd_local, center_framex, center_framey, center_pointedframex, center_pointedframey, pointed_MAG, t_pointed_count2, pointed_count2, diff_centerx, diff_centery, pointed_pixelx[npointed_count2], pointed_pixely[npointed_count2], npointed_count2);

				Mat ROI(frame, Rect(center_framex - frame_width / 2 * MAG_thrd_local, center_framey - frame_height / 2 * MAG_thrd_local, frame_width * MAG_thrd_local, frame_height * MAG_thrd_local));
				ROI.copyTo(Cut_frame);
				resize(Cut_frame, frame2, Size(IMG_WIDTH * MAG_IMG, frame_height * IMG_WIDTH * MAG_IMG / frame_width), 0, 0, INTER_AREA);
				//cout << "IMG_WIDTH_CENTETR=" << Size(IMG_WIDTH * MAG_IMG, frame_height * IMG_WIDTH * MAG_IMG / frame_width) << endl;

				if (MAG_ExpandedIMG > 4) {
					line(frame2, Point(100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(1100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "1 mm", Point(400 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 4, LINE_AA);
				}
				else if (MAG_ExpandedIMG <= 4 && MAG_ExpandedIMG > 2) {

					line(frame2, Point(80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(180 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "100 um", Point(70 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				else if (MAG_ExpandedIMG <= 2 && MAG_ExpandedIMG > 1.4) {

					line(frame2, Point(60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(110 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "50 um", Point(50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				else if (MAG_ExpandedIMG <= 1.4) {

					line(frame2, Point(70 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 30 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(90 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 30 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "20 um", Point(50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 25 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 2 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				for (int npointed_count2 = 0; npointed_count2 < pointed_count2; npointed_count2++) {
					rectangle(frame2, Point(diff_centerx - (double)(IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img, diff_centery - (double)(IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img), Point(diff_centerx + (double)(IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img, diff_centery + (double)(IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img), Scalar(0, 0, 0), 3, LINE_AA);
					//rectangle(frame2, Point(diff_centerx - (double)IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width, diff_centery - (double)IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width), Point(diff_centerx + (double)IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width, diff_centery + (double)IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width), Scalar(0, 0, 0), 3, LINE_AA);
				}
			}
			//nstoreimageをまたぐとき
			else if (t_store_image_count < t_expand_viewpoint_num - 1) {
				if (t_count_Xndata >= t_store_image_count) {
					if (count_nstoreimage > 0) {
						for (int count_store_image = nstoreimage + t_store_image_count - t_count_Xndata; count_store_image < nstoreimage; count_store_image++) {

							//contrast2(pimage[count_store_image], contrast_pimage);
							//create_img(pimage[count_store_image], frame);
							create_image(z_in, pangle[count_store_image], pimage[count_store_image], frame, Depth, 0., resolution);
						}
						for (int count_store_image = 0; count_store_image < t_store_image_count + 1; count_store_image++) {

							//contrast2(pimage[count_store_image], contrast_pimage);
							//create_img(pimage[count_store_image], frame);
							create_image(z_in, pangle[count_store_image], pimage[count_store_image], frame, Depth, 0., resolution);
						}
					}
				}

				ClTest.expanded_CV_rect_setting(MAG_thrd_local, center_framex, center_framey, mouse_posx, mouse_posy);
				ClTest.pointed_rectangle_setting(MAG_thrd_local, center_framex, center_framey, center_pointedframex, center_pointedframey, pointed_MAG, t_pointed_count2, pointed_count2, diff_centerx, diff_centery, pointed_pixelx[npointed_count2], pointed_pixely[npointed_count2], npointed_count2);

				Mat ROI(frame, Rect(center_framex - frame_width / 2 * MAG_thrd_local, center_framey - frame_height / 2 * MAG_thrd_local, frame_width * MAG_thrd_local, frame_height * MAG_thrd_local));
				ROI.copyTo(Cut_frame);
				resize(Cut_frame, frame2, Size(IMG_WIDTH * MAG_IMG, frame_height * IMG_WIDTH * MAG_IMG / frame_width), 0, 0, INTER_AREA);

				if (MAG_ExpandedIMG > 4) {
					line(frame2, Point(100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(1100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "1 mm", Point(400 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 4, LINE_AA);
				}
				else if (MAG_ExpandedIMG <= 4 && MAG_ExpandedIMG > 2) {

					line(frame2, Point(80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(180 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "100 um", Point(70 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				else if (MAG_ExpandedIMG <= 2 && MAG_ExpandedIMG > 1.4) {

					line(frame2, Point(60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(110 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "50 um", Point(50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				else if (MAG_ExpandedIMG <= 1.4) {

					line(frame2, Point(70 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 30 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(90 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 30 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "20 um", Point(50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 25 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 2 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				for (int npointed_count2 = 0; npointed_count2 < pointed_count2; npointed_count2++) {
					rectangle(frame2, Point(diff_centerx - (double)(IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img, diff_centery - (double)(IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img), Point(diff_centerx + (double)(IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img, diff_centery + (double)(IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img), Scalar(0, 0, 0), 3, LINE_AA);
					//rectangle(frame2, Point(diff_centerx - (double)IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width, diff_centery - (double)IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width), Point(diff_centerx + (double)IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width, diff_centery + (double)IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width), Scalar(0, 0, 0), 3, LINE_AA);
				}

			}

			frame2.copyTo(ClTest.Xframe1);
			ClTest.Xframe_out[0] = 1;


		}
		QueryPerformanceCounter(&frame_qpc2);
		time_Xcrt1 = ((double)frame_qpc2.QuadPart - (double)frame_qpc1.QuadPart) / (double)qpf.QuadPart * 1000;
		//cout << "th1_rooptime =" << time_Xcrt1 << endl;

	}

	sleep(1000);
	return 0;
}

//マルチスレッド4　視野拡張画像合成と表示
unsigned __stdcall mythread4(LPVOID lpx)
{
	Mat frame2;	//拡張画像
	//Mat frame3(frame_height * IMG_WIDTH * MAG_IMG / frame_width, IMG_WIDTH * MAG_IMG + IMG_WIDTH2, CV_8UC1); //選択した画像
	//namedWindow(window_name);
	////moveWindow(window_name, IMG_WIDTH + 600, 50);
	//moveWindow(window_name, windowframe_xpos - 9, windowframe_ypos - 32);	//初期設定：表示するウィンドウをモニタ中心に移動させる。（常に表示位置を固定したい場合はfor文中に設定する）  MAG_IMGが大きすぎるときの対策するべき

	//setMouseCallback(window_name, mouse_callback, &isClick);
	double MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;
	double t_MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;

	double MAG_thrd_local = MAG;
	int t_store_image_count = 0;
	int t_count_Xndata = 0;
	int t_expand_viewpoint_num = expand_viewpoint_num;
	int pointed_count2 = pointed_count;
	int t_pointed_count2 = pointed_count2;
	Mat frame(frame_height, frame_width, CV_8UC1);
	Mat Depth(frame_height, frame_width, CV_8UC1);
	center_framex = frame_width / 2;
	center_framey = frame_height / 2;
	LARGE_INTEGER frame_qpc1;
	LARGE_INTEGER frame_qpc2;
	double time_frame = 0;
	double t_time_frame = time_frame;
	//int count_update = 0;
	double magnification2 = magnification;
	//int expand_viewpoint_num2 = expand_viewpoint_num;
	//int t_expand_viewpoint_num2 = expand_viewpoint_num2;
	int center_pointedframex = center_framex;
	int center_pointedframey = center_framey;
	double pointed_MAG = MAG;
	int s_flag = 1;


	//QueryPerformanceCounter(&frame_qpc1);
	WaitForSingleObject(hEvent[4], INFINITE);

	for (;;)
	{
		//printf("スレッド4");
		SuspendThread(hTh[4]);
		if (end_flag == 1) break;
		QueryPerformanceCounter(&frame_qpc1);
		//cout << "th4_start" << endl;

		if (switch_flag == 1)
		{
			//拡張画像合成に用いる画像番号の取得
			ClTest.load_ExpCount_data(t_store_image_count, t_count_Xndata, t_expand_viewpoint_num, MAG_thrd_local);
			//cout << store_image_count2 << "," << count_Xndata2 << endl;
			Xcrt_exp_vp_num[1] = t_expand_viewpoint_num;


			//sleep(100);
			//if (thread_start_num < 15) sleep(300);
			t_pointed_count2 = pointed_count2;
			pointed_count2 = pointed_count;

			//t_expand_viewpoint_num2 = expand_viewpoint_num2;
			//expand_viewpoint_num2 = expand_viewpoint_num;

			Mat Cut_frame;
			frame2 = Scalar(0);
			double diff_centerx = 0;
			double diff_centery = 0;

			//合成に用いる画像番号がnstoreimage(確保している画像情報のメモリ領域（画像枚数）)をまたぐときと，またがないときで分岐が必要（後者のみでも動作はするが，またいだときに生成されないため画面が一瞬暗転する）
			//nstoreimageをまたがないとき
			if (t_store_image_count >= t_expand_viewpoint_num - 1) {
				for (int count_store_image = t_store_image_count - t_expand_viewpoint_num + 1; count_store_image < t_store_image_count + 1; count_store_image++) {
					//for (int count_store_image = store_image_count2-expand_viewpoint_num+1+8; count_store_image < store_image_count2 - expand_viewpoint_num + 2+8; count_store_image++) {	//一枚だけ配置したいとき（検証用）
						//cout << "A:" << count_store_image << ",s:"<< store_image_count2 - expand_viewpoint_num + 1 << ",e:" << store_image_count2 + 1 <<  endl;
					create_image(z_in, pangle[count_store_image], pimage[count_store_image], frame, Depth, 0., resolution);
					//contrast2(pimage[count_store_image], contrast_pimage);
					//create_image(z_in, pangle[count_store_image - 1], pimage[count_store_image], frame, Depth, 0., resolution);
					//count_update++;
					//printf("%d,mirror_anglex=%f,mirror_angley=%f\n", count_store_image, *pangle[count_store_image], *(pangle[count_store_image]+1));
				}


				ClTest.expanded_CV_rect_setting(MAG_thrd_local, center_framex, center_framey, mouse_posx, mouse_posy);
				ClTest.pointed_rectangle_setting(MAG_thrd_local, center_framex, center_framey, center_pointedframex, center_pointedframey, pointed_MAG, t_pointed_count2, pointed_count2, diff_centerx, diff_centery, pointed_pixelx[npointed_count2], pointed_pixely[npointed_count2], npointed_count2);

				Mat ROI(frame, Rect(center_framex - frame_width / 2 * MAG_thrd_local, center_framey - frame_height / 2 * MAG_thrd_local, frame_width * MAG_thrd_local, frame_height * MAG_thrd_local));
				ROI.copyTo(Cut_frame);
				resize(Cut_frame, frame2, Size(IMG_WIDTH * MAG_IMG, frame_height * IMG_WIDTH * MAG_IMG / frame_width), 0, 0, INTER_AREA);

				if (MAG_ExpandedIMG > 4) {
					line(frame2, Point(100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(1100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "1 mm", Point(400 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 4, LINE_AA);
				}
				else if (MAG_ExpandedIMG <= 4 && MAG_ExpandedIMG > 2) {

					line(frame2, Point(80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(180 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "100 um", Point(70 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				else if (MAG_ExpandedIMG <= 2 && MAG_ExpandedIMG > 1.4) {

					line(frame2, Point(60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(110 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "50 um", Point(50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				else if (MAG_ExpandedIMG <= 1.4) {

					line(frame2, Point(70 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 30 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(90 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 30 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "20 um", Point(50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 25 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 2 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				for (int npointed_count2 = 0; npointed_count2 < pointed_count2; npointed_count2++) {
					rectangle(frame2, Point(diff_centerx - (double)(IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img, diff_centery - (double)(IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img), Point(diff_centerx + (double)(IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img, diff_centery + (double)(IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img), Scalar(0, 0, 0), 3, LINE_AA);
					//rectangle(frame2, Point(diff_centerx - (double)IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width, diff_centery - (double)IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width), Point(diff_centerx + (double)IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width, diff_centery + (double)IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width), Scalar(0, 0, 0), 3, LINE_AA);
				}

			}
			//nstoreimageをまたぐとき
			else if (t_store_image_count < t_expand_viewpoint_num - 1) {
				if (t_count_Xndata >= t_store_image_count) {
					if (count_nstoreimage > 0) {
						for (int count_store_image = nstoreimage + t_store_image_count - t_count_Xndata; count_store_image < nstoreimage; count_store_image++) {
							//contrast2(pimage[count_store_image], contrast_pimage);
							//create_img(pimage[count_store_image], frame);
							create_image(z_in, pangle[count_store_image], pimage[count_store_image], frame, Depth, 0., resolution);
						}
						for (int count_store_image = 0; count_store_image < t_store_image_count + 1; count_store_image++) {

							//contrast2(pimage[count_store_image], contrast_pimage);
							//create_img(pimage[count_store_image], frame);
							create_image(z_in, pangle[count_store_image], pimage[count_store_image], frame, Depth, 0., resolution);
						}
					}
				}

				ClTest.expanded_CV_rect_setting(MAG_thrd_local, center_framex, center_framey, mouse_posx, mouse_posy);
				ClTest.pointed_rectangle_setting(MAG_thrd_local, center_framex, center_framey, center_pointedframex, center_pointedframey, pointed_MAG, t_pointed_count2, pointed_count2, diff_centerx, diff_centery, pointed_pixelx[npointed_count2], pointed_pixely[npointed_count2], npointed_count2);

				Mat ROI(frame, Rect(center_framex - frame_width / 2 * MAG_thrd_local, center_framey - frame_height / 2 * MAG_thrd_local, frame_width * MAG_thrd_local, frame_height * MAG_thrd_local));
				ROI.copyTo(Cut_frame);
				resize(Cut_frame, frame2, Size(IMG_WIDTH * MAG_IMG, frame_height * IMG_WIDTH * MAG_IMG / frame_width), 0, 0, INTER_AREA);

				if (MAG_ExpandedIMG > 4) {
					line(frame2, Point(100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(1100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "1 mm", Point(400 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 4, LINE_AA);
				}
				else if (MAG_ExpandedIMG <= 4 && MAG_ExpandedIMG > 2) {

					line(frame2, Point(80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(180 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "100 um", Point(70 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				else if (MAG_ExpandedIMG <= 2 && MAG_ExpandedIMG > 1.4) {

					line(frame2, Point(60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(110 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "50 um", Point(50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				else if (MAG_ExpandedIMG <= 1.4) {

					line(frame2, Point(70 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 30 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(90 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 30 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "20 um", Point(50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 25 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 2 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				for (int npointed_count2 = 0; npointed_count2 < pointed_count2; npointed_count2++) {
					rectangle(frame2, Point(diff_centerx - (double)(IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img, diff_centery - (double)(IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img), Point(diff_centerx + (double)(IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img, diff_centery + (double)(IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img), Scalar(0, 0, 0), 3, LINE_AA);
					//rectangle(frame2, Point(diff_centerx - (double)IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width, diff_centery - (double)IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width), Point(diff_centerx + (double)IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width, diff_centery + (double)IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width), Scalar(0, 0, 0), 3, LINE_AA);
				}

			}

			frame2.copyTo(ClTest.Xframe2);
			ClTest.Xframe_out[1] = 1;
			QueryPerformanceCounter(&frame_qpc2);
			time_Xcrt2 = ((double)frame_qpc2.QuadPart - (double)frame_qpc1.QuadPart) / (double)qpf.QuadPart * 1000;
			//cout << "th4_rooptime =" << time_Xcrt2 << endl;
		}
	}
	sleep(1000);
	return 0;
}

//マルチスレッド4　視野拡張画像合成と表示
unsigned __stdcall mythread6(LPVOID lpx)
{
	Mat frame2;	//拡張画像
	//Mat frame3(frame_height * IMG_WIDTH * MAG_IMG / frame_width, IMG_WIDTH * MAG_IMG + IMG_WIDTH2, CV_8UC1); //選択した画像
	//namedWindow(window_name);
	////moveWindow(window_name, IMG_WIDTH + 600, 50);
	//moveWindow(window_name, windowframe_xpos - 9, windowframe_ypos - 32);	//初期設定：表示するウィンドウをモニタ中心に移動させる。（常に表示位置を固定したい場合はfor文中に設定する）  MAG_IMGが大きすぎるときの対策するべき

	//setMouseCallback(window_name, mouse_callback, &isClick);
	double MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;
	double t_MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;
	double MAG_thrd_local = MAG;


	int t_store_image_count = 0;
	int t_count_Xndata = 0;
	int t_expand_viewpoint_num = expand_viewpoint_num;

	int pointed_count2 = pointed_count;
	int t_pointed_count2 = pointed_count2;
	Mat frame(frame_height, frame_width, CV_8UC1);
	Mat Depth(frame_height, frame_width, CV_8UC1);
	center_framex = frame_width / 2;
	center_framey = frame_height / 2;
	LARGE_INTEGER frame_qpc1;
	LARGE_INTEGER frame_qpc2;
	double time_frame = 0;
	double t_time_frame = time_frame;
	//int count_update = 0;
	double magnification2 = magnification;
	//int t_expand_viewpoint_num2 = expand_viewpoint_num2;
	int center_pointedframex = center_framex;
	int center_pointedframey = center_framey;
	double pointed_MAG = MAG;
	int s_flag = 1;


	//QueryPerformanceCounter(&frame_qpc1);
	WaitForSingleObject(hEvent[6], INFINITE);


	for (;;) {

		SuspendThread(hTh[6]);
		if (end_flag == 1) break;
		QueryPerformanceCounter(&frame_qpc1);
		//cout << "th6_start" << endl;

		if (switch_flag == 1) {
			//拡張画像合成に用いる画像番号の取得
			ClTest.load_ExpCount_data(t_store_image_count, t_count_Xndata, t_expand_viewpoint_num, MAG_thrd_local);
			//cout << store_image_count2 << "," << count_Xndata2 << endl;
			Xcrt_exp_vp_num[2] = t_expand_viewpoint_num;

			//printf("スレッド6");

			//sleep(100);
			//if (thread_start_num < 15) sleep(300);

			t_pointed_count2 = pointed_count2;
			pointed_count2 = pointed_count;

			//t_expand_viewpoint_num2 = expand_viewpoint_num2;
			//expand_viewpoint_num2 = expand_viewpoint_num;

			Mat Cut_frame;
			frame2 = Scalar(0);
			double diff_centerx = 0;
			double diff_centery = 0;

			//合成に用いる画像番号がnstoreimage(確保している画像情報のメモリ領域（画像枚数）)をまたぐときと，またがないときで分岐が必要（後者のみでも動作はするが，またいだときに生成されないため画面が一瞬暗転する）
			//nstoreimageをまたがないとき
			if (t_store_image_count >= t_expand_viewpoint_num - 1) {
				//if (t_store_image_count >= expand_viewpoint_num - 1) {
				//	for (int count_store_image = t_store_image_count - expand_viewpoint_num + 1; count_store_image < t_store_image_count + 1; count_store_image++) {
				for (int count_store_image = t_store_image_count - t_expand_viewpoint_num + 1; count_store_image < t_store_image_count + 1; count_store_image++) {
					//for (int count_store_image = store_image_count2-expand_viewpoint_num+1+8; count_store_image < store_image_count2 - expand_viewpoint_num + 2+8; count_store_image++) {	//一枚だけ配置したいとき（検証用）
						//cout << "A:" << count_store_image << ",s:"<< store_image_count2 - expand_viewpoint_num + 1 << ",e:" << store_image_count2 + 1 <<  endl;
					create_image(z_in, pangle[count_store_image], pimage[count_store_image], frame, Depth, 0., resolution);
					//contrast2(pimage[count_store_image], contrast_pimage);
					//create_image(z_in, pangle[count_store_image - 1], pimage[count_store_image], frame, Depth, 0., resolution);
					//count_update++;
					//printf("%d,mirror_anglex=%f,mirror_angley=%f\n", count_store_image, *pangle[count_store_image], *(pangle[count_store_image]+1));
				}



				ClTest.expanded_CV_rect_setting(MAG_thrd_local, center_framex, center_framey, mouse_posx, mouse_posy);
				ClTest.pointed_rectangle_setting(MAG_thrd_local, center_framex, center_framey, center_pointedframex, center_pointedframey, pointed_MAG, t_pointed_count2, pointed_count2, diff_centerx, diff_centery, pointed_pixelx[npointed_count2], pointed_pixely[npointed_count2], npointed_count2);

				Mat ROI(frame, Rect(center_framex - frame_width / 2 * MAG_thrd_local, center_framey - frame_height / 2 * MAG_thrd_local, frame_width * MAG_thrd_local, frame_height * MAG_thrd_local));
				ROI.copyTo(Cut_frame);
				resize(Cut_frame, frame2, Size(IMG_WIDTH * MAG_IMG, frame_height * IMG_WIDTH * MAG_IMG / frame_width), 0, 0, INTER_AREA);

				if (MAG_ExpandedIMG > 4) {
					line(frame2, Point(100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(1100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "1 mm", Point(400 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 4, LINE_AA);
				}
				else if (MAG_ExpandedIMG <= 4 && MAG_ExpandedIMG > 2) {

					line(frame2, Point(80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(180 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "100 um", Point(70 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				else if (MAG_ExpandedIMG <= 2 && MAG_ExpandedIMG > 1.4) {

					line(frame2, Point(60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(110 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "50 um", Point(50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				else if (MAG_ExpandedIMG <= 1.4) {

					line(frame2, Point(70 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 30 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(90 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 30 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "20 um", Point(50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 25 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 2 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				for (int npointed_count2 = 0; npointed_count2 < pointed_count2; npointed_count2++) {
					rectangle(frame2, Point(diff_centerx - (double)(IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img, diff_centery - (double)(IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img), Point(diff_centerx + (double)(IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img, diff_centery + (double)(IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img), Scalar(0, 0, 0), 3, LINE_AA);
					//rectangle(frame2, Point(diff_centerx - (double)IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width, diff_centery - (double)IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width), Point(diff_centerx + (double)IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width, diff_centery + (double)IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width), Scalar(0, 0, 0), 3, LINE_AA);
				}

			}
			//nstoreimageをまたぐとき
			else if (t_store_image_count < t_expand_viewpoint_num - 1) {
				if (t_count_Xndata >= t_store_image_count) {
					if (count_nstoreimage > 0) {
						for (int count_store_image = nstoreimage + t_store_image_count - t_count_Xndata; count_store_image < nstoreimage; count_store_image++) {

							//contrast2(pimage[count_store_image], contrast_pimage);
							//create_img(pimage[count_store_image], frame);
							create_image(z_in, pangle[count_store_image], pimage[count_store_image], frame, Depth, 0., resolution);
						}
						for (int count_store_image = 0; count_store_image < t_store_image_count + 1; count_store_image++) {

							//contrast2(pimage[count_store_image], contrast_pimage);
							//create_img(pimage[count_store_image], frame);
							create_image(z_in, pangle[count_store_image], pimage[count_store_image], frame, Depth, 0., resolution);
						}
					}
				}




				ClTest.expanded_CV_rect_setting(MAG_thrd_local, center_framex, center_framey, mouse_posx, mouse_posy);
				ClTest.pointed_rectangle_setting(MAG_thrd_local, center_framex, center_framey, center_pointedframex, center_pointedframey, pointed_MAG, t_pointed_count2, pointed_count2, diff_centerx, diff_centery, pointed_pixelx[npointed_count2], pointed_pixely[npointed_count2], npointed_count2);

				Mat ROI(frame, Rect(center_framex - frame_width / 2 * MAG_thrd_local, center_framey - frame_height / 2 * MAG_thrd_local, frame_width * MAG_thrd_local, frame_height * MAG_thrd_local));
				ROI.copyTo(Cut_frame);
				resize(Cut_frame, frame2, Size(IMG_WIDTH * MAG_IMG, frame_height * IMG_WIDTH * MAG_IMG / frame_width), 0, 0, INTER_AREA);

				if (MAG_ExpandedIMG > 4) {
					line(frame2, Point(100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(1100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 100 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "1 mm", Point(400 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 4, LINE_AA);
					b_width = 1000 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution;
				}
				else if (MAG_ExpandedIMG <= 4 && MAG_ExpandedIMG > 2) {

					line(frame2, Point(80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(180 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 80 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "100 um", Point(70 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				else if (MAG_ExpandedIMG <= 2 && MAG_ExpandedIMG > 1.4) {

					line(frame2, Point(60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(110 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 60 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "50 um", Point(50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 3 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				else if (MAG_ExpandedIMG <= 1.4) {

					line(frame2, Point(70 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 30 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), Point(90 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 30 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), (255, 255, 255), 4, LINE_AA, 0);
					putText(frame2, "20 um", Point(50 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, 25 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution), FONT_HERSHEY_PLAIN, 2 / MAG_thrd_local * MAG_IMG * IMG_WIDTH / frame_width / resolution, cv::Scalar(255, 255, 255), 2, LINE_AA);

				}

				for (int npointed_count2 = 0; npointed_count2 < pointed_count2; npointed_count2++) {
					rectangle(frame2, Point(diff_centerx - (double)(IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img, diff_centery - (double)(IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img), Point(diff_centerx + (double)(IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img, diff_centery + (double)(IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width) / mag_small_img), Scalar(0, 0, 0), 3, LINE_AA);
					//rectangle(frame2, Point(diff_centerx - (double)IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width, diff_centery - (double)IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width), Point(diff_centerx + (double)IMG_WIDTH / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width, diff_centery + (double)IMG_HEIGHT / 2 * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification2 / frame_width), Scalar(0, 0, 0), 3, LINE_AA);
				}
				cout << "b_frame=" << b_framex << "," << b_framey << endl;
			}
			b_framex = (double)IMG_WIDTH * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification / frame_width;
			b_framey = (double)IMG_HEIGHT * IMG_WIDTH * MAG_IMG / MAG_thrd_local * pixelpitch / resolution / magnification / frame_width;
			//ex_normalx = b_framex / 2;
			//ex_normaly = b_framey / 2;
			diff_centerx_s = diff_centerx;
			diff_centery_s = diff_centery;

			//cout << "diff_center_s=" << diff_centerx_s << "," << diff_centery_s << endl;


			frame2.copyTo(ClTest.Xframe3);
			ClTest.Xframe_out[2] = 1;
			QueryPerformanceCounter(&frame_qpc2);
			time_Xcrt3 = ((double)frame_qpc2.QuadPart - (double)frame_qpc1.QuadPart) / (double)qpf.QuadPart * 1000;
			//cout << "th6_rooptime =" << time_Xcrt3 << endl;
			;
		}
	}
	sleep(1000);
	return 0;
}

//マルチスレッド4　視野拡張画像表示
unsigned __stdcall mythread5(LPVOID lpx)
{

	//Mat frame3(IMG_WIDTH * MAG_IMG, frame_height * IMG_WIDTH * MAG_IMG / frame_width, CV_8UC1);	

	/*呈示する拡張画像の設定*/
	Mat Xframe(IMG_WIDTH * MAG_IMG, frame_height * IMG_WIDTH * MAG_IMG / frame_width, CV_8UC1); //呈示する拡張画像
	//Mat Xframe(IMG_WIDTH * MAG_IMG, frame_height * IMG_WIDTH * MAG_IMG / frame_width, CV_8U); //呈示する拡張画像
	frame_height2 = frame_height * IMG_WIDTH * MAG_IMG / frame_width;
	frame_width2 = IMG_WIDTH * MAG_IMG;
	namedWindow(window_name);
	moveWindow(window_name, windowframe_xpos - 9, windowframe_ypos - 32);	//初期設定：表示するウィンドウをモニタ中心に移動させる。(tobiiによる視線計測を行う場合はopencv設定上での配置位置と実際にモニタ上に配置される位置のズレに注意すること）
	setMouseCallback(window_name, mouse_callback, &isClick);		//マウスコールバック設定



	XimageCreationTime << "th1 disp time" << "," << "th2 disp time" << "," << "th3 disp time" << "," << "th1 crt time" << "," << "th2 crt time" << "," << "th3 crt time" << "," << "exp_vp_num1" << "," << "exp_vp_num2" << "," << "exp_vp_num3" << endl;

	//QueryPerformanceCounter(&frame_qpc1);
	WaitForSingleObject(hEvent[5], INFINITE);
	double disp_X1_time = 0;
	double disp_X2_time = 0;
	double disp_X3_time = 0;
	LARGE_INTEGER myth5_qpc0;
	LARGE_INTEGER disp_X1;
	LARGE_INTEGER disp_X2;
	LARGE_INTEGER disp_X3;
	LARGE_INTEGER th5_start;
	LARGE_INTEGER th5_end;
	double th5_rooptime;
	QueryPerformanceCounter(&myth5_qpc0);

	for (;;)
	{
		int DA_Data_auto;
		int dauto = 500;
		double man4_auto_z;
		//cout << "thread5_start" << endl;
	start:QueryPerformanceCounter(&th5_start);


		if (switch_flag == 1)
		{
			//拡張画像合成スレッドで画像が生成された時，フレームの上書きを実行する．
			if (ClTest.Xframe_out[0] == 1) {
				ClTest.Xframe1.copyTo(Xframe);
				ClTest.Xframe_out[0] = 0;
				QueryPerformanceCounter(&disp_X1);
				disp_X1_time = ((double)disp_X1.QuadPart - (double)myth5_qpc0.QuadPart) / (double)qpf.QuadPart * 1000;
			}

			if (ClTest.Xframe_out[1] == 1) {
				ClTest.Xframe2.copyTo(Xframe);
				ClTest.Xframe_out[1] = 0;

				QueryPerformanceCounter(&disp_X2);
				disp_X2_time = ((double)disp_X2.QuadPart - (double)myth5_qpc0.QuadPart) / (double)qpf.QuadPart * 1000;
			}

			if (ClTest.Xframe_out[2] == 1) {
				ClTest.Xframe3.copyTo(Xframe);
				ClTest.Xframe_out[2] = 0;
				QueryPerformanceCounter(&disp_X3);
				disp_X3_time = ((double)disp_X3.QuadPart - (double)myth5_qpc0.QuadPart) / (double)qpf.QuadPart * 1000;
				XimageCreationTime << disp_X1_time << "," << disp_X2_time << "," << disp_X3_time << "," << time_Xcrt1 << "," << time_Xcrt2 << "," << time_Xcrt3 << "," << Xcrt_exp_vp_num[0] << "," << Xcrt_exp_vp_num[1] << "," << Xcrt_exp_vp_num[2] << endl;


			}

			//操作感覚計測時ポイント表示 2024/5/27
			if (m_flag == 1) {
				cv::circle(Xframe, cv::Point((int)man1_mpoint2D_ex[0], (int)man1_mpoint2D_ex[1]), 2, cv::Scalar(0, 200, 0), 3);
				cv::circle(Xframe, cv::Point((int)man1_cposx_s, (int)man1_cposy_s), 2, cv::Scalar(0, 200, 0), 3);
				//cv::circle(Xframe, cv::Point((int)point_man_x, (int)point_man_y), 2, cv::Scalar(0, 200, 0), 3);
			}
			else if (man1_flag >= point_number && m_flag == 0) {
				cv::circle(Xframe, cv::Point((int)man1_cpos_ex[0], (int)man1_cpos_ex[1]), 2, cv::Scalar(0, 200, 0), 3);
				//cv::circle(circle_img, cv::Point((int)man1_pos2D.at<double>(0, 0), (int)man1_pos2D.at<double>(0, 1)), 2, cv::Scalar(200, 0, 0), 3);
			}

			//ピペット先端位置の描写
			cv::circle(Xframe, Point((int)t_2Dpos_ex[0], (int)t_2Dpos_ex[1]), 2, Scalar(255, 255, 0), 5);//変換確認用
			//man1の描写
			//if (man1_flag >= point_number) {
			//	cv::circle(Xframe, cv::Point((int)man1_cpos_ex[0], (int)man1_cpos_ex[1]), 2, cv::Scalar(0, 200, 0), 3);
			//	//cv::circle(circle_img, cv::Point((int)man1_pos2D.at<double>(0, 0), (int)man1_pos2D.at<double>(0, 1)), 2, cv::Scalar(200, 0, 0), 3);
			//}

			//man2の描写
			if (man2_flag >= point_number) {
				cv::circle(Xframe, cv::Point((int)man2_cpos_ex[0], (int)man2_cpos_ex[1]), 2, cv::Scalar(0, 200, 0), 3);
				//cv::circle(circle_img, cv::Point((int)man2_pos2D.at<double>(0, 0), (int)man2_pos2D.at<double>(0, 1)), 2, cv::Scalar(200, 0, 0), 3);
			}

			//man3の描写
			if (man3_flag >= point_number) {
				cv::circle(Xframe, cv::Point((int)man3_cpos_ex[0], (int)man3_cpos_ex[1]), 2, cv::Scalar(0, 200, 0), 3);
				//cv::circle(circle_img, cv::Point((int)man2_pos2D.at<double>(0, 0), (int)man2_pos2D.at<double>(0, 1)), 2, cv::Scalar(200, 0, 0), 3);
			}
			//man4の描写
			if (man4_flag >= point_number) {
				cv::circle(Xframe, cv::Point((int)man4_cpos_ex[0], (int)man4_cpos_ex[1]), 1, cv::Scalar(0, 200, 0), 3);
				//cv::circle(circle_img, cv::Point((int)man2_pos2D.at<double>(0, 0), (int)man2_pos2D.at<double>(0, 1)), 2, cv::Scalar(200, 0, 0), 3);
			}

			//半自動搬送の位置を画像上で確認
			set_pos12d_ex[0] = 623.0 + (173.0 / 623.0) * (set_pos12d[0] - 623.0);
			set_pos12d_ex[1] = 504.0 + (144.0 / 504.0) * (set_pos12d[1] - 504.0);
			set_pos22d_ex[0] = 623.0 + (173.0 / 623.0) * (set_pos22d[0] - 623.0);
			set_pos22d_ex[1] = 504.0 + (144.0 / 504.0) * (set_pos22d[1] - 504.0);
			//cv::circle(Xframe, cv::Point((int)set_pos12d_ex[0], (int)set_pos12d_ex[1]), 2, cv::Scalar(0, 255, 0), 3);
			//cv::circle(Xframe, cv::Point((IMG_WIDTH * MAG_IMG)/2, (frame_height * IMG_WIDTH * MAG_IMG / frame_width)/2), 2, cv::Scalar(0, 255, 0), 3);
			//rectangle(Xframe, Point((IMG_WIDTH * MAG_IMG) / 2 - (b_framex/2), ((frame_height * IMG_WIDTH * MAG_IMG / frame_width) / 2)-(b_framey/2)), Point((IMG_WIDTH * MAG_IMG) / 2 + (b_framex/2), ((frame_height * IMG_WIDTH * MAG_IMG / frame_width) / 2) + (b_framey/2)), Scalar(0, 0, 0), 1, LINE_AA);
			//cv::circle(Xframe, cv::Point(diff_centerx_s, diff_centery_s), 2, cv::Scalar(0, 0, 0), 3);
			//rectangle(Xframe, cv::Point(man2_cpos_ex[0], man2_cpos_ex[1]-25), cv::Point(man2_cpos_ex[0]+50, man2_cpos_ex[1]+25), Scalar(0, 0, 0), 1, LINE_AA);

			//int ker = 9;
			////画像鮮鋭化
			//cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
			//	-ker / 9, -ker / 9, -ker / 9,
			//	-ker / 9, 1 + (8 * ker / 9), -ker / 9,
			//	-ker / 9, -ker / 9, -ker / 9);
			//cv::Mat dest;
			//cv::filter2D(Xframe, dest, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

			//imshow("dest", dest);
			imshow(window_name, Xframe);
			waitKey(1);//waitKeyがないと表示されない

			Xframe.copyTo(t_frame);

			//円検出
			if (GetAsyncKeyState(0x44))//円検出モード切り替え D
			{
				c_detection = 1;

			}

			if (c_detection == 1)
			{
				//cout << "円検出モード" << endl;
				Mat threshold_img_ex;
				int threshold_limitx;
				int threshold_limity1;
				int threshold_limity2;
				//Rect myROI(4, 4, IMG_WIDTH, IMG_HEIGHT);
				//Mat rect_view = Xframe(myROI);
				threshold(t_frame, threshold_img_ex, Threshold, 255, THRESH_BINARY_INV);

				//マニピュレータより上を隠す
				//if (man1_cpos_ex[1] > man2_cpos_ex[1]) threshold_limit = int(man1_cpos_ex[1]+50);
				//else threshold_limit = int(man2_cpos_ex[1]);


				//20250308 ピペットが衝突しないように範囲を制限
				if (active_flag == 4) {
					if (center_flag == 1) {
						threshold_limity2 = man1_cpos_ex[1] + 40;
						threshold_limitx = man1_cpos_ex[0] - 350;
					}
					else if (center_flag == 0) {
						threshold_limity2 = pre_man24_cpos_ex[1] + 40;
						threshold_limitx = pre_man24_cpos_ex[0] - 100;
					}
					//threshold_limity2 = man4_cpos_ex[1] + dyy;
					//threshold_limitx = man4_cpos_ex[0] - 100;
					//threshold_limitx = man1_cpos_ex[0] - 350;

					for (int y = man4_cpos_ex[1] - 30; y < man4_cpos_ex[1] + 30; y++) {
						for (int x = 0; x < man4_cpos_ex[0] + 10; x++) {
							threshold_img_ex.data[IMG_WIDTH * y + x] = 0;
						}
					}
					threshold_limity1 = 230;
				}
				else if (active_flag == 2) {
					if (center_flag == 1) {
						threshold_limity1 = man1_cpos_ex[1] - 120;
						threshold_limitx = man1_cpos_ex[0] - 350;
					}
					else if (center_flag == 0) {
						threshold_limity1 = pre_man24_cpos_ex[1] - 120;
						threshold_limitx = pre_man24_cpos_ex[0] - 100;
					}

					for (int y = man2_cpos_ex[1] - 30; y < man2_cpos_ex[1] + 30; y++) {
						for (int x = 0; x < man2_cpos_ex[0] + 10; x++) {
							threshold_img_ex.data[IMG_WIDTH * y + x] = 0;
						}
					}
					threshold_limity2 = 750;
				}
				else {
					threshold_limitx = 800;
					threshold_limity1 = 230;
					threshold_limity2 = 750;
				}

				//cout << "limit =" << threshold_limitx << "," << threshold_limity1 << "," << threshold_limity2 << endl;

				for (int y = 0; y < 900; y++) {
					for (int x = threshold_limitx; x < IMG_WIDTH; x++) {
						threshold_img_ex.data[IMG_WIDTH * y + x] = 0;
					}
				}
				for (int y = 0; y < threshold_limity1; y++) {
					for (int x = 0; x < IMG_WIDTH; x++) {
						threshold_img_ex.data[IMG_WIDTH * y + x] = 0;
					}
				}
				for (int y = threshold_limity2; y < 900; y++) {
					for (int x = 0; x < IMG_WIDTH; x++) {
						threshold_img_ex.data[IMG_WIDTH * y + x] = 0;
					}
				}


				left_circle_number = 0;

				//ラベリング処理
				cv::Mat LabelImg;
				cv::Mat stats;
				cv::Mat centroids;
				int nLab = cv::connectedComponentsWithStats(threshold_img_ex, LabelImg, stats, centroids);
				NOC = 0;
				for (int i = 1; i < nLab; ++i) {
					int* param = stats.ptr<int>(i);
					int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
					int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];

					if (height < dia_max_ex && height > dia_min_ex && width < dia_max_ex && width > dia_min_ex) {
						int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
						int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
						center_x[NOC] = x + width / 2;
						center_y[NOC] = y + height / 2;
						radius[NOC] = (width + height) / 4.0;
						cv::circle(t_frame, cv::Point(center_x[NOC], center_y[NOC]), (int)radius[NOC], cv::Scalar(0, 0, 255), 2);
						NOC = NOC + 1;
					}
					if (NOC == LIMIT) break;
					imshow("c_detection", t_frame);
					//std::cout << "NOC=" << NOC << std::endl;
				}
				if (left_circle_flag == 1) {
					for (int i = 0; i < NOC; ++i) {
						if (center_x[left_circle_number] > center_x[i]) {
							left_circle_number = i;
						}
						cout << "center_x" << i << "=" << center_x[i] << endl;
					}
					cout << "left_circle_number=" << left_circle_number << endl;
					left_circle_x = center_x[left_circle_number];
					left_circle_y = center_y[left_circle_number];
					//t_2Dpos_ex[0] = left_circle_x;
					//t_2Dpos_ex[1] = left_circle_y; //6/13
					NOC_auto = NOC;
					left_circle_flag = 0;
					imwrite("detection_frame.png", threshold_img_ex);
					waitKey(1);
				}

				imshow("threshold_img", threshold_img_ex);
			}
		}
		if (end_flag == 1) {
			XimageCreationTime.close();
			ResumeThread(hTh[1]);
			ResumeThread(hTh[4]);
			ResumeThread(hTh[5]);
			destroyAllWindows();
			break;
		}
		QueryPerformanceCounter(&th5_end);
		th5_rooptime = ((double)th5_end.QuadPart - (double)th5_start.QuadPart) / (double)qpf.QuadPart * 1000;
		//cout << "th5_rooptime =" << th5_rooptime << endl;
	}
	sleep(1000);
	return 0;
}


//マルチスレッド２　操作者によって選択された視点の画像処理と表示
unsigned __stdcall mythread2(LPVOID lpx)
{
	LARGE_INTEGER pointed_frame_qpc1;
	double time_pointed_frame = 0;
	double t_time_pointed_frame = time_pointed_frame;
	int store_pointed_image_count2 = 0;
	int t_store_pointed_image_count2 = store_pointed_image_count2;
	int pointed_count2 = 0;
	int s_flag = 0;
	int t_pointed_count2 = 0;
	LARGE_INTEGER th2_start;
	LARGE_INTEGER th2_end;
	double th2_rooptime;

	//int img_width2, img_height2;
	for (;;)
	{
		//cout << "th2_start" << endl;
	start:QueryPerformanceCounter(&th2_start);
		if (switch_flag == 1)
		{
			//printf("スレッド2");
			WaitForSingleObject(hEvent[2], INFINITE);
			//printf("a");
			if (t_pointed_count2 < pointed_count2) t_pointed_count2 = pointed_count2;
			pointed_count2 = pointed_count;
			//printf("pointed_count=%d",pointed_count2);
			for (int npointed_count = 0; npointed_count < pointed_count2; npointed_count++) {

				//t_store_pointed_image_count2 = store_pointed_image_count2;
				while (t_store_pointed_image_count2 >= store_pointed_image_count2) {
					store_pointed_image_count2 = store_pointed_image_count;
					if (t_store_pointed_image_count2 > store_pointed_image_count2) {
						t_store_pointed_image_count2 = store_pointed_image_count2;
					}
					sleep(18);  //20241029 変更 12→
				}
				t_store_pointed_image_count2 = store_pointed_image_count2;
				//printf("store_pointed_count=%d", store_pointed_image_count2);
				if (store_pointed_image_count2 > 2) {
					Mat pointed_frame(IMG_HEIGHT, IMG_WIDTH, CV_8UC1, ppointedimage[nstoreimage * npointed_count + store_pointed_image_count2 - 2]);
					Mat pointed_framex;
					Mat small_img_res;
					cv::Mat small_img2(Size((int)(IMG_WIDTH * mag_pointed_frame), IMG_HEIGHT * mag_pointed_frame), CV_8UC3);
					cv::flip(pointed_frame, pointed_framex, 0);//0上下,1左右,-1両方反転

					Kc = 1246 * 0.8 / (b_framex / mag_small_img);
					//double Kc_y = 1008 * 0.8 / (b_framey / mag_small_img);

					//計測用　拡大視野にも座標を表示
					man1_cpos_c[0] = (IMG_WIDTH * 0.4) + (Kc * (man1_cpos_ex[0] - point_man_x_s));
					man1_cpos_c[1] = (IMG_HEIGHT * 0.4) + (Kc * (man1_cpos_ex[1] - point_man_y_s));

					//cv::circle(pointed_framex, cv::Point((int)man1_mpoint2D_c[0], (int)man1_mpoint2D_c[1]), 2, cv::Scalar(0, 200,0), 3);
					//cout << "man1_cpos_c = " << man1_cpos_c[0] << "," << man1_cpos_c[1] << endl;
					//cout << "man1_cpos_ex = " << man1_cpos_ex[0] << "," << man1_cpos_ex[1] << endl;
					//cout << "point_man_x_s = " << point_man_x_s << "," << point_man_y_s << endl;
					//cout << "Kc = " << Kc << endl;
					cv::circle(small_img2, cv::Point((int)man1_cpos_c[0], (int)man1_cpos_c[1]), 2, cv::Scalar(0, 0, 160), 3);
					cv::rectangle(pointed_framex, cv::Point(IMG_WIDTH * (0.4 - (1 / (2 * mag_small_img))), IMG_HEIGHT * (0.4 - (1 / (2 * mag_small_img)))), cv::Point(IMG_WIDTH * (0.4 + (1 / (2 * mag_small_img))), IMG_HEIGHT * (0.4 + (1 / (2 * mag_small_img)))), cv::Scalar(255, 0, 0), 3);//岐阜大と同じサイズの画像を表示

					//保持中の卵子の円表示
					if (inside_flag > 0) {
						C_center_xc2 = (IMG_WIDTH * 0.4) + (Kc * (C_center_x - point_man_x_s));
						C_center_yc2 = (IMG_HEIGHT * 0.4) + (Kc * (C_center_y - point_man_y_s));
						C_radius_c2 = C_radius_ex * Kc;
					}

					//cout << "C_center = " << C_center_xc << "," << C_center_yc << endl;
					//cout << "C_radius_c = " << C_radius_c << endl;
					//cv::circle(small_img2, cv::Point((int)C_center_xc2, (int)C_center_yc2), C_radius_c2, cv::Scalar(0, 200, 0), 2);
					//cv::circle(small_img, cv::Point((int)C_center_xc, (int)C_center_yc), C_radius_c, cv::Scalar(0, 200, 0), 2);

					//スリットの位置を計算
					if (inside_flag == 2) {
						slit_xc = (IMG_WIDTH * 0.4) + (Kc * (slit_x - point_man_x_s));
						slit_yc = (IMG_HEIGHT * 0.4) + (Kc * (slit_y - point_man_y_s));
						cv::circle(small_img2, cv::Point((int)slit_xc, (int)slit_yc), 4, cv::Scalar(0, 200, 0), -1);
					}
					else if (inside_flag > 2) {
						cv::circle(small_img2, cv::Point((int)slit_xc, (int)slit_yc), 5, cv::Scalar(0, 0, 120), -1);
					}

					//namedWindow(pointed_window_name);
					//moveWindow(pointed_window_name, img_width2, img_height2);
					//imshow(pointed_window_name, pointed_framex);
					imshow("small", small_img2);
					//imwrite("small_superres.png", small_img2);
					waitKey(1);

					//small_img.copyTo(small_img2);

					//選択した画像の保存（shift + Sボタン）
					//imwrite("sample_img\\microbeads\\1s.png", pointed_framex);	//選択した画像
					if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(0x53)) {
						ostringstream oss;
						oss << p2_i;
						imwrite("sample_img\\microbeads\\" + oss.str() + ".png", pointed_framex);
						//imwrite("sample_img\\microbeads\\2s.png", pointed_framex);	//選択した画像
						p2_i += 1;
					}
					waitKey(1);//waitKeyがないと表示されない
					//printf("npointed_count=%d\n", npointed_count);
				}
			}

			QueryPerformanceCounter(&pointed_frame_qpc1);

			t_time_pointed_frame = time_pointed_frame;
			time_pointed_frame = ((double)pointed_frame_qpc1.QuadPart - (double)qpc0.QuadPart) / (double)qpf.QuadPart * 1000;
			double fps = 1000 / (time_pointed_frame - t_time_pointed_frame);
			//printf("pointed_count=%d,store_pointed_image_count2=%d,time=%f, FPS=%f\n", pointed_count, store_pointed_image_count2, time_pointed_frame, fps);


			for (int npointed_count = pointed_count2; npointed_count < t_pointed_count2; npointed_count++) {
				sprintf(pointed_window_name, "pointed_frame%d", npointed_count);
				//destroyWindow(pointed_window_name);
				destroyWindow("small");
				//printf(pointed_window_name);
				//printf("pointed_count=%d, pointed_count2=%d,t_pointed_count2=%d\n", pointed_count, pointed_count2, t_pointed_count2);
				t_pointed_count2 = pointed_count2;
			}

			if (pointed_count2 != t_pointed_count2 && pointed_count2 > 0) s_flag = 1;
			else if (pointed_count2 != t_pointed_count2 && pointed_count2 == 0) {
				s_t2count2 = 0;
				s_t2count1++;
				s_flag = 0;
			}

			if (s_flag == 1) {
				s_pointed_count[s_t2count1][s_t2count2] = pointed_count2;
				s_t2fps[s_t2count1][s_t2count2] = fps;
				s_t2time_frame[s_t2count1][s_t2count2] = time_pointed_frame;
				s_t2count2++;
				sc_t2count2[s_t2count1] = s_t2count2;

				if (s_t2count2 > 99) {
					s_t2count2 = 0;
					s_t2count1++;
					if (s_t2count1 > 1023) s_t2count1 = 1023;
					s_flag = 0;
				}
			}

			int MaxID = 0;

			//スリットの位置を取得
			if (GetAsyncKeyState(0x52) && GetAsyncKeyState(0x32)) {
				if (inside_flag == 0) {
					Get_circle_size(C_radius, C_center_x, C_center_y, man2_cpos_ex);
					C_radius_ex = C_radius * radius_ratio;
					inside_flag = 1;
					cout << "C_radius=" << C_radius << endl;
					cout << "C_center_x=" << C_center_x << endl;
					cout << "C_center_y=" << C_center_y << endl;
				}
				else if (inside_flag > 0) {
					inside_flag = 0;
				}

				//cout << "inside_flag=" << inside_flag << endl;

				sleep(100);
			}
			else if (GetAsyncKeyState(0x52) && GetAsyncKeyState(0x34)) {
				if (inside_flag == 0) {
					Get_circle_size(C_radius, C_center_x, C_center_y, man4_cpos_ex);
					C_radius_ex = C_radius * radius_ratio;
					inside_flag = 1;
					cout << "C_radius=" << C_radius << endl;
					cout << "C_center_x=" << C_center_x << endl;
					cout << "C_center_y=" << C_center_y << endl;
				}
				else if (inside_flag > 0) {
					inside_flag = 0;
				}
				sleep(100);
			}

			//スリットの位置を取得(萩原AI)
			if (GetAsyncKeyState(0x52) && GetAsyncKeyState(0x35)) {
				if (inside_flag == 0) {
					Get_circle_size(C_radius, C_center_x, C_center_y, man2_cpos_ex);
					C_radius_ex = C_radius * radius_ratio;
					man24_cpos_c[0] = (IMG_WIDTH * 0.4) + (Kc * (man2_cpos_ex[0] - point_man_x_s));
					man24_cpos_c[1] = (IMG_HEIGHT * 0.4) + (Kc * (man2_cpos_ex[1] - point_man_y_s));
					transparency();

					//最大の領域のみ抽出
					Mat stats;
					Mat centroids;
					int nLab = connectedComponentsWithStats(bg, Labelmg_slit, stats, centroids);
					int Max = 0;

					cout << "nLab =" << nLab << endl;

					for (int i = 1; i < nLab; ++i) {//全てのラベルを回す
						int* param = stats.ptr<int>(i);
						if (Max < param[ConnectedComponentsTypes::CC_STAT_AREA]) {//最大値より大きければ
							Max = param[ConnectedComponentsTypes::CC_STAT_AREA];//最大値を更新
							MaxID = i;
							trimmed_area = Max;

						}
					}

					int* param = stats.ptr<int>(MaxID);
					int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
					int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
					int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
					int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
					C_center_xc = x + width / 2;
					C_center_yc = y + height / 2;
					C_radius_c = (width + height) / 4.0;
					cout << "Ccenter =" << C_center_xc << "," << C_center_yc << endl;
					cout << "C_radius=" << C_radius_c << endl;
					cout << "Max_ID =" << MaxID << endl;
					sleep(100);
					inside_flag = 3;
				}
				else if (inside_flag > 0) {
					inside_flag = 0;
				}
				sleep(100);
			}
			else if (GetAsyncKeyState(0x52) && GetAsyncKeyState(0x36)) { //スリット位置固定
				inside_flag = 5;
				sleep(100);
			}
			//else if (GetAsyncKeyState(0x52) && GetAsyncKeyState(0x37)) { //力覚提示
			//	inside_flag = 6;
			//	sleep(100);
			//}

			//体細胞挿入支援
			if (inside_flag == 1) {
				if (pow(man1_cpos_ex[0] - C_center_x, 2) + pow(man1_cpos_ex[1] - C_center_y, 2) < pow(C_radius_ex, 2) && man1_cpos_ex[0] > C_center_x) {
					slit_x = man1_cpos_ex[0];
					slit_y = man1_cpos_ex[1];
					cout << "slit_pos=" << slit_x << "," << slit_y << endl;
					inside_flag = 2;
				}
				cv::circle(t_frame, cv::Point(C_center_x, C_center_y), (int)C_radius_ex, cv::Scalar(0, 0, 255), 2);
				//sleep(1);
			}
			else if (inside_flag == 2) {
				if (pow(man1_cpos_ex[0] - C_center_x, 2) + pow(man1_cpos_ex[1] - C_center_y, 2) > pow(C_radius_ex, 2) && man1_cpos_ex[0] > C_center_x) inside_flag = 1;
				else cout << "inside" << endl;
				cv::circle(t_frame, cv::Point(C_center_x, C_center_y), (int)C_radius_ex, cv::Scalar(0, 0, 255), 2);
				//sleep(1);
			}
			else if (inside_flag == 3) {
				//cout << "Labeling_label=" << Labelmg_slit.at<int>(man1_cpos_c[1], man1_cpos_c[0]) << endl;
				if (0 < man1_cpos_c[0] && man1_cpos_c[0] < bg.cols && 0 < man1_cpos_c[1] && man1_cpos_c[0] < bg.rows) {
					if (Labelmg_slit.at<int>(man1_cpos_c[1], man1_cpos_c[0]) > 0) {
						slit_xc = man1_cpos_c[0];
						slit_yc = man1_cpos_c[1];
						cout << "slit_pos2=" << slit_xc << "," << slit_yc << endl;
						inside_flag = 4;
					}
				}
			}
			else if (inside_flag == 4) {
				//if (pow(man1_cpos_c[0] - C_center_xc2, 2) + pow(man1_cpos_c[1] - C_center_yc2, 2) > pow(C_radius_c2, 2) && man1_cpos_c[0] > C_center_xc2) inside_flag = 3;
				if (pow(man1_xw - man13_store[0] + 130, 2) + pow(man1_yw - man13_store[1], 2) > pow(70, 2) && man1_cpos_c[0] > C_center_xc2) inside_flag = 3;
				else cout << "inside2" << endl;
				cout << "man1_cpos_c =" << man1_cpos_c[0] << "," << man1_cpos_c[1] << endl;
				cout << " C_center=" << C_center_xc2 << "," << C_center_yc2 << endl;
				cout << " C_radius=" << C_radius_c2 << endl;
				//cv::circle(small_img, cv::Point(C_center_x, C_center_y), (int)C_radius_c, cv::Scalar(0, 0, 255), 2);
				//sleep(1);
			}

			//man13_store[0] - 300, man13_store[1], cent_pos[2]

			//cout << "inside_flag =" << inside_flag << endl;

			//sleep(20);

		}
		if (end_flag == 1) {

			destroyAllWindows();
			break;
		}
		QueryPerformanceCounter(&th2_end);
		th2_rooptime = ((double)th2_end.QuadPart - (double)th2_start.QuadPart) / (double)qpf.QuadPart * 1000;
		//cout << "th2_rooptime =" << th2_rooptime << endl;
	}
	sleep(1000);
	return 0;
}

//指定した位置を拡大
void zoom(double pos_x, double pos_y) {
	int center_framex2 = center_framex;
	int center_framey2 = center_framey;
	double MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;//最初の視野と現在の視野の割合
	double magnification2 = magnification;

	//移動先を勝手に拡大
	phantom_flag = 2;
	//インジェクションピペット周辺の切り抜き
	point_man_x = pos_x;
	point_man_y = pos_y;
	point_man_x_s = point_man_x;
	point_man_y_s = point_man_y;

	pointed_pixelx[pointed_count] = point_man_x;
	pointed_pixely[pointed_count] = point_man_y;

	//pointed_pixelx[pointed_count] = x;
	//pointed_pixely[pointed_count] = y;		
	pointed_mirrorx[pointed_count] = -atan((double)((point_man_x - IMG_WIDTH * MAG_IMG / 2) * MAG * resolution * frame_width / (IMG_WIDTH * MAG_IMG) + (double(center_framex2) - frame_width / 2) * resolution) / ((l2 + l3) * 1000 - z_in)) / 2 * 180 / PI;
	pointed_mirrory[pointed_count] = atan((double)((point_man_y - frame_height * double(IMG_WIDTH) * MAG_IMG / frame_width / 2) * MAG * resolution * frame_height / (frame_height * double(IMG_WIDTH) * MAG_IMG / frame_width) + (double(center_framey2) - frame_height / 2) * resolution) / ((l3) * 1000 - z_in)) / 2 * 180 / PI;
	pointed_WD[pointed_count] = derive_WD(pointed_mirrorx[pointed_count] * 2, pointed_mirrory[pointed_count] * 2);
	t_pointed_count = pointed_count;
	pointed_count++;
	if (pointed_count > max_pointed_count) pointed_count = 0;

	for (int npointed_count = 0; npointed_count < pointed_count; npointed_count++) {

		if (pointed_WD[npointed_count] > pointed_WD[pointed_count - 1]) {
			double a_line, b_line, c_line, d_line, e_line;
			a_line = pointed_pixelx[pointed_count - 1];
			b_line = pointed_pixely[pointed_count - 1];
			c_line = pointed_mirrorx[pointed_count - 1];
			d_line = pointed_mirrory[pointed_count - 1];
			e_line = pointed_WD[pointed_count - 1];
			pointed_pixelx[pointed_count - 1] = pointed_pixelx[npointed_count];
			pointed_pixely[pointed_count - 1] = pointed_pixely[npointed_count];
			pointed_mirrorx[pointed_count - 1] = pointed_mirrorx[npointed_count];
			pointed_mirrory[pointed_count - 1] = pointed_mirrory[npointed_count];
			pointed_WD[pointed_count - 1] = pointed_WD[npointed_count];
			pointed_pixelx[npointed_count] = a_line;
			pointed_pixely[npointed_count] = b_line;
			pointed_mirrorx[npointed_count] = c_line;
			pointed_mirrory[npointed_count] = d_line;
			pointed_WD[npointed_count] = e_line;
		}
	}
	center_flag = 0;
	phase_2_flag = 0;
}

void zoom_cancel() {
	t_pointed_count = pointed_count;
	phantom_flag = 1;
	for (int npointed_count = 0; npointed_count < pointed_count; npointed_count++) {
		//doubleにキャストしないとオーバーフローして選択解除できなくなる（IMG_WとIMG_Hが小さい場合はキャストしなくてもオーバーフローしない）
		if (npointed_count < pointed_count - 1) {
			for (int npointed_count2 = npointed_count; npointed_count2 < pointed_count - 1; npointed_count2++) {
				pointed_pixelx[npointed_count2] = pointed_pixelx[npointed_count2 + 1];
				pointed_pixely[npointed_count2] = pointed_pixely[npointed_count2 + 1];
				pointed_mirrorx[npointed_count2] = pointed_mirrorx[npointed_count2 + 1];
				pointed_mirrory[npointed_count2] = pointed_mirrory[npointed_count2 + 1];
				pointed_WD[npointed_count2] = pointed_WD[npointed_count2 + 1];
			}
			npointed_count--;
			pointed_count--;
		}
		if (npointed_count == pointed_count - 1) {
			npointed_count--;
			pointed_count--;
		}
	}
	center_flag = 1;
}

//マルチスレッド３　操作者からの入力受付
unsigned __stdcall mythread3(LPVOID lpx)
{
	ofstream data_file("time.csv");
	double work_time;

	int DA_Data_auto;
	int dauto = 700;
	int timer_flag = 0;
	int work_count = 1;
	int auto_end = 0;
	int max_hold = 1;//取りに行く回数の最大
	Mat hold_frame;
	Mat th_hold_frame;//切り抜き二値化画像

	LARGE_INTEGER qpc_start;
	LARGE_INTEGER qpc_end;

	for (;;) {

		//視野拡張と通常視野の切り替え(sボタン+1ボタン)
		if (GetAsyncKeyState(0x53)) {
			if (GetAsyncKeyState(0x31) && switch_flag == 2) {
				switch_flag = 1;
				sleep(100);
				//destroyAllWindows();
				waitKey(1);

				cout << "destroy1" << endl;
				sleep(3100);
			}

			//通常視野へ(sボタン+2ボタン)
			if (GetAsyncKeyState(0x32) && switch_flag == 1) {
				switch_flag = 2;
				sleep(100);
				//destroyAllWindows();
				waitKey(1);

				//円検出
				Mat threshold_img_ex;
				//Rect myROI(4, 4, IMG_WIDTH, IMG_HEIGHT);
				//Mat rect_view = Xframe(myROI);
				threshold(t_frame, threshold_img_ex, Threshold, 255, THRESH_BINARY_INV);

				//ラベリング処理
				cv::Mat LabelImg;
				cv::Mat stats;
				cv::Mat centroids;

				int nLab = cv::connectedComponentsWithStats(threshold_img_ex, LabelImg, stats, centroids);
				int NOC = 0;
				move_count = 0;
				for (int i = 1; i < nLab; ++i) {
					int* param = stats.ptr<int>(i);
					int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
					int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];

					if (height < dia_max && height > dia_min && width < dia_max && width > dia_min) {
						int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
						int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
						center_x[NOC] = x + width / 2;
						center_y[NOC] = y + height / 2;
						radius[NOC] = (width + height) / 4.0;
						cv::circle(t_frame, cv::Point(center_x[NOC], center_y[NOC]), (int)radius[NOC], cv::Scalar(0, 0, 255), 2);
						NOC = NOC + 1;
					}
					if (NOC == LIMIT) break;
					imshow("c_detection", t_frame);
					//std::cout << "NOC=" << NOC << std::endl;
				}
				cout << "destroy2" << endl;
				sleep(1100);
			}
		}
		//フレームないでピペットの位置がほぼ変わらないように
		//if (move_mode == 2) {
		//	Pcenter_dx_ex = man1_cpos_ex[0] - (frame_width2 / 2);
		//	Pcenter_dy_ex = man1_cpos_ex[1] - (frame_height2 / 2);
		//	Pcenter_dx = Pcenter_dx_ex / (IMG_WIDTH * 0.8 / b_framex);
		//	Pcenter_dy = Pcenter_dy_ex / (IMG_HEIGHT * 0.8 / b_framey);
		//	point_man_x = man1_cpos_ex[0] - Pcenter_dx;
		//	point_man_y = man1_cpos_ex[1] - Pcenter_dy;
		//}
		//else if (move_mode == 1) {
		//	Pcenter_dx_ex = man3_cpos_ex[0] - (frame_width2 / 2);
		//	Pcenter_dy_ex = man3_cpos_ex[1] - (frame_height2 / 2);
		//	Pcenter_dx = Pcenter_dx_ex / (IMG_WIDTH * 0.8 / b_framex);
		//	Pcenter_dy = Pcenter_dy_ex / (IMG_HEIGHT * 0.8 / b_framey);
		//	point_man_x = man3_cpos_ex[0] - Pcenter_dx;
		//	point_man_y = man3_cpos_ex[1] - Pcenter_dy;
		//}
		if (move_mode == 2) {
			point_man_x = man1_cpos_ex[0] - 40;
			point_man_y = man1_cpos_ex[1];
		}
		else if (move_mode == 1) {
			point_man_x = man3_cpos_ex[0] - 40;
			point_man_y = man3_cpos_ex[1];
		}

		//cout << " point_man = " << point_man_x << "," << point_man_y << endl;
		//cout << " man1_cpos_ex = " << man1_cpos_ex[0] << "," << man1_cpos_ex[1] << endl;
		//cout << " Pcenter_d = " << Pcenter_dx << "," << Pcenter_dy << endl;

		int center_framex2 = center_framex;
		int center_framey2 = center_framey;
		double MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;//最初の視野と現在の視野の割合
		double magnification2 = magnification;
		//ふっとペダルで変更したい
		if (GetAsyncKeyState(0x55)) {	//U 視野拡張
			if (center_flag == 0) { //選択していない時
				t_pointed_count = pointed_count;
				phantom_flag = 1;
				for (int npointed_count = 0; npointed_count < pointed_count; npointed_count++) {
					//doubleにキャストしないとオーバーフローして選択解除できなくなる（IMG_WとIMG_Hが小さい場合はキャストしなくてもオーバーフローしない）
					if (npointed_count < pointed_count - 1) {
						for (int npointed_count2 = npointed_count; npointed_count2 < pointed_count - 1; npointed_count2++) {
							pointed_pixelx[npointed_count2] = pointed_pixelx[npointed_count2 + 1];
							pointed_pixely[npointed_count2] = pointed_pixely[npointed_count2 + 1];
							pointed_mirrorx[npointed_count2] = pointed_mirrorx[npointed_count2 + 1];
							pointed_mirrory[npointed_count2] = pointed_mirrory[npointed_count2 + 1];
							pointed_WD[npointed_count2] = pointed_WD[npointed_count2 + 1];
						}
						npointed_count--;
						pointed_count--;
					}
					if (npointed_count == pointed_count - 1) {
						npointed_count--;
						pointed_count--;
					}
				}
				center_flag = 1;
			}
			else if (center_flag == 1) { //通常視野
				phantom_flag = 2;
				//インジェクションピペット周辺の切り抜き
				point_man_x_s = point_man_x;
				point_man_y_s = point_man_y;

				pointed_pixelx[pointed_count] = point_man_x;
				pointed_pixely[pointed_count] = point_man_y;

				//pointed_pixelx[pointed_count] = x;
				//pointed_pixely[pointed_count] = y;		
				pointed_mirrorx[pointed_count] = -atan((double)((point_man_x - IMG_WIDTH * MAG_IMG / 2) * MAG * resolution * frame_width / (IMG_WIDTH * MAG_IMG) + (double(center_framex2) - frame_width / 2) * resolution) / ((l2 + l3) * 1000 - z_in)) / 2 * 180 / PI;
				pointed_mirrory[pointed_count] = atan((double)((point_man_y - frame_height * double(IMG_WIDTH) * MAG_IMG / frame_width / 2) * MAG * resolution * frame_height / (frame_height * double(IMG_WIDTH) * MAG_IMG / frame_width) + (double(center_framey2) - frame_height / 2) * resolution) / ((l3) * 1000 - z_in)) / 2 * 180 / PI;
				pointed_WD[pointed_count] = derive_WD(pointed_mirrorx[pointed_count] * 2, pointed_mirrory[pointed_count] * 2);
				t_pointed_count = pointed_count;
				pointed_count++;
				if (pointed_count > max_pointed_count) pointed_count = 0;

				for (int npointed_count = 0; npointed_count < pointed_count; npointed_count++) {

					if (pointed_WD[npointed_count] > pointed_WD[pointed_count - 1]) {
						double a_line, b_line, c_line, d_line, e_line;
						a_line = pointed_pixelx[pointed_count - 1];
						b_line = pointed_pixely[pointed_count - 1];
						c_line = pointed_mirrorx[pointed_count - 1];
						d_line = pointed_mirrory[pointed_count - 1];
						e_line = pointed_WD[pointed_count - 1];
						pointed_pixelx[pointed_count - 1] = pointed_pixelx[npointed_count];
						pointed_pixely[pointed_count - 1] = pointed_pixely[npointed_count];
						pointed_mirrorx[pointed_count - 1] = pointed_mirrorx[npointed_count];
						pointed_mirrory[pointed_count - 1] = pointed_mirrory[npointed_count];
						pointed_WD[pointed_count - 1] = pointed_WD[npointed_count];
						pointed_pixelx[npointed_count] = a_line;
						pointed_pixely[npointed_count] = b_line;
						pointed_mirrorx[npointed_count] = c_line;
						pointed_mirrory[npointed_count] = d_line;
						pointed_WD[npointed_count] = e_line;
					}
				}
				center_flag = 0;
			}
			cout << " center_flag : " << center_flag << endl;
			sleep(10);

		}

		//printf("スレッド3");
		WaitForSingleObject(hEvent[4], INFINITE);

		// 大島追加
		// ★ AIの「実行」キー (Shift + D)
		if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(0x44)) { // 'D'
			if (!g_ai_motor_engaged.load()) { // まだ実行中でない場合
				std::cout << "[AI] Capture request sent (Shift+D)." << std::endl;
				g_ai_injection_done.store(false); // ★追加: 新しい試行のときは完了フラグを消す
				// ★ AI初期画像キャプチャを要求 (mythread2 が実行)
				g_ai_capture_request.store(true);
			}
			sleep(300); // チャタリング防止
		}
		// ★ AIの「停止」キー (Shift + S)
		if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(0x53)) { // 'S'
			if (g_ai_motor_engaged.load()) {
				g_ai_motor_engaged.store(false);
				g_ai_progress.store(0.0);
				g_ai_is_stop.store(false);
				std::cout << "[AI] Motor Disengaged. AI Reset." << std::endl;
				// ★ 高速版のためのリセット処理を追加
				g_ai_hidden_h.reset();
				g_ai_hidden_c.reset();
				g_locked_roi_width.store(0);  // ★ ロックしたROIをリセット
				g_locked_roi_height.store(0); // ★ ロックしたROIをリセット
				g_ai_initial_frame_mat.release(); // ★ 前のフレームもリセット
				g_ai_stop_counter.store(0); // ★ 連続STOPカウンターもリセット
			}
			sleep(300);
		}
		//追加ここまで

		//if (GetAsyncKeyState(VK_LEFT)) posy++;
		//if (GetAsyncKeyState(VK_RIGHT)) posy--;
		//if (posy < 0)posy = 0;
		if (GetAsyncKeyState(VK_DOWN) && switch_flag == 1) {
			if (phase_flag > 0) { //体細胞保持フェーズ以降
				s_dWD2 = s_dWD2 + 0.0025;
				printf("s_dWD=%f", s_dWD2);
				//ピペットを焦点に合わせて移動　09/04 5は適当
				//man3_zw = man3_zw - 1.16;
				//std::vector<double> movepos{ man3_xw,man3_yw,man3_zw + 2 };
				//Movepos(hDevice_RD, s, movepos);
			}
			else {
				//s_dWD = s_dWD + 0.001;
				s_dWD2 = s_dWD2 + 0.0025;
				printf("s_dWD=%f", s_dWD2);
			}
		}

		if (GetAsyncKeyState(VK_UP) && switch_flag == 1) {
			if (phase_flag > 0) { //体細胞保持フェーズ以降
				s_dWD2 = s_dWD2 - 0.0025;
				printf("s_dWD=%f", s_dWD2);
				//ピペットを焦点に合わせて移動　09/04 5は適当
				//man3_zw = man3_zw + 1.16;
			}
			else {
				//s_dWD = s_dWD - 0.001;
				s_dWD2 = s_dWD2 - 0.0025;
				printf("s_dWD=%f", s_dWD2);
			}
		}

		//if (GetAsyncKeyState(VK_RIGHT)) {
		//	Threshold = Threshold + 1;
		//	printf("Threshold=%d", Threshold);
		//}
		//if (GetAsyncKeyState(VK_LEFT)) {
		//	Threshold = Threshold - 1;
		//	printf("Threshold=%d", Threshold);
		//}

		if (GetAsyncKeyState(0x36)) {
			s_mirror_y = s_mirror_y + 0.01;
			cout << "s_mirror_y=%d" << s_mirror_y << endl;
			sleep(10);
		}
		if (GetAsyncKeyState(0x35)) {
			s_mirror_y = s_mirror_y - 0.01;
			cout << "s_mirror_y=%d" << s_mirror_y << endl;
			sleep(10);
		}

		if (GetAsyncKeyState(VK_RIGHT)) {
			s_mirror_x = s_mirror_x + 0.01;
			cout << "s_mirror_x=%d" << s_mirror_x << endl;
			sleep(10);
		}

		if (GetAsyncKeyState(VK_LEFT)) {
			s_mirror_x = s_mirror_x - 0.01;
			cout << "s_mirror_x=%d" << s_mirror_x << endl;
			sleep(10);
		}

		//大島追加
		//oshima
		// ↓↓↓ ★★★ Modbusモーター制御ロジックを以下のように変更 ★★★ ↓↓↓
		if (g_ai_motor_engaged.load())
		{
			// --- 1. AIによる自動制御 (Motor1) ---
			bool is_stop = g_ai_is_stop.load(); // mythread2 がセットしたフラグ

			if (is_stop) {
				current_speed1 = 0;
				current_direction1 = 0;
				printf("[AI] Stop detected. Reversing 200 steps.\n");
				// ★ [変更] 戻るべき合計ステップ数を取得
				long steps_to_return = g_total_moved_steps.load();
				// ★ [変更] 1秒間で戻るための速度計算 (速度 = 距離 / 時間)
				// ステップ数が0の場合は安全のため最小速度を設定
				double return_speed = (steps_to_return > 0) ? (double)steps_to_return : 100.0;
				// 逆回転 (-steps_to_return) を 算出速度(return_speed) で実行
				std::thread thread_m1_rev(moveStepRelative, std::ref(*motor1), -steps_to_return, return_speed, "Motor1");
				g_ai_injection_done.store(true);
				//std::thread thread_m1_rev(moveStepRelative, std::ref(*motor1), -100, 200, "Motor1");
				thread_m1_rev.detach();

				// ★ 自動停止したら、AI制御モードも解除
				g_ai_motor_engaged.store(false);
				g_ai_is_stop.store(false);
				g_ai_progress.store(0.0);
				g_ai_hidden_h.reset();
				g_ai_hidden_c.reset();
				g_locked_roi_width.store(0);  // ★ ロックしたROIをリセット
				g_locked_roi_height.store(0); // ★ ロックしたROIをリセット
				g_ai_initial_frame_mat.release(); // ★ 前のフレームもリセット
				g_ai_stop_counter.store(0); // ★ 連続STOPカウンターもリセット

				// ★ カウンタと積算ステップのリセット
				g_ai_drive_counter.store(0);
				g_total_moved_steps.store(0);
				g_last_pred_class.store(-1);
				
			}
			else {
				// ★ AIが「実行」を指示 (0.95未満)
			}

			// (AI制御中はMotor2 (Home/End) は手動で動かせるようにしておく)
			if (GetAsyncKeyState(VK_HOME) & 0x8000) {
				if (current_speed2 != 100 || current_direction2 != 1) {
					current_speed2 = 100;
					current_direction2 = 1;
					std::thread thread_m2_f(move, std::ref(*motor2), current_speed2, current_direction2, "Motor2");
					thread_m2_f.detach();
					printf("Motor2 Forward\n");
				}
			}
			else if (GetAsyncKeyState(VK_END) & 0x8000) {
				if (current_speed2 != 100 || current_direction2 != 2) {
					current_speed2 = 100;
					current_direction2 = 2;
					std::thread thread_m2_r(move, std::ref(*motor2), current_speed2, current_direction2, "Motor2");
					thread_m2_r.detach();
					printf("Motor2 Reverse\n");
				}
			}
			else {
				if (current_speed2 != 0) {
					current_speed2 = 0;
					current_direction2 = 0;
					std::thread thread_m2_s(stop, std::ref(*motor2), "Motor2");
					thread_m2_s.detach();
					printf("Motor2 Stop\n");
				}
			}
		}
		else{
			// --- 2. 従来の手動制御 (AIが OFF の場合) ---

			// Motor1 制御 (Insert/Deleteキー)
			if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
				if (current_speed1 != 50 || current_direction1 != 1) {
					current_speed1 = 50;
					current_direction1 = 1;
					std::thread thread_m1_f(move, std::ref(*motor1), current_speed1, current_direction1, "Motor1");
					thread_m1_f.detach();
					printf("Motor1 Forward\n");
				}
			}
			else if (GetAsyncKeyState(VK_DELETE) & 0x8000) {
				if (current_speed1 != 50 || current_direction1 != 2) {
					current_speed1 = 50;
					current_direction1 = 2;
					std::thread thread_m1_r(move, std::ref(*motor1), current_speed1, current_direction1, "Motor1");
					thread_m1_r.detach();
					printf("Motor1 Reverse\n");
				}
			}
			else {
				if (current_speed1 != 0) {
					current_speed1 = 0;
					current_direction1 = 0;
					std::thread thread_m1_s(stop, std::ref(*motor1), "Motor1");
					thread_m1_s.detach();
					printf("Motor1 Stop\n");
				}
			}

			// Motor2 制御 (Home/Endキー)
			if (GetAsyncKeyState(VK_HOME) & 0x8000) {
				if (current_speed2 != 100 || current_direction2 != 1) {
					current_speed2 = 100;
					current_direction2 = 1;
					std::thread thread_m2_f(move, std::ref(*motor2), current_speed2, current_direction2, "Motor2");
					thread_m2_f.detach();
					printf("Motor2 Forward\n");
				}
			}
			else if (GetAsyncKeyState(VK_END) & 0x8000) {
				if (current_speed2 != 100 || current_direction2 != 2) {
					current_speed2 = 100;
					current_direction2 = 2;
					std::thread thread_m2_r(move, std::ref(*motor2), current_speed2, current_direction2, "Motor2");
					thread_m2_r.detach();
					printf("Motor2 Reverse\n");
				}
			}
			else {
				if (current_speed2 != 0) {
					current_speed2 = 0;
					current_direction2 = 0;
					std::thread thread_m2_s(stop, std::ref(*motor2), "Motor2");
					thread_m2_s.detach();
					printf("Motor2 Stop\n");
				}
			}

			// F10キー：設定されたステップ数だけ「正転」
			if (GetAsyncKeyState(VK_F10) & 0x8000) {
				if (current_speed1 == 0) { // 連続運転中でないことを確認
					// moveStepRelative を別スレッドで実行
					std::thread thread_f(moveStepRelative, std::ref(*motor1), g_step_count, g_step_speed, "Motor1");
					thread_f.detach();
					std::this_thread::sleep_for(std::chrono::milliseconds(300)); // 重複入力防止
				}
			}

			// F11キー：設定されたステップ数だけ「逆転」
			else if (GetAsyncKeyState(VK_F11) & 0x8000) {
				if (current_speed1 == 0) {
					// ステップ数にマイナス（-）をつけて逆転させる
					std::thread thread_r(moveStepRelative, std::ref(*motor1), -g_step_count, g_step_speed_2, "Motor1");
					thread_r.detach();
					std::this_thread::sleep_for(std::chrono::milliseconds(300));
				}
			}
		}
		// Shiftキーが押されているか確認
		bool isShiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
		// ▼▼▼▼▼▼▼ 緊急停止 (Backspaceキー) ▼▼▼▼▼▼▼
		if (GetAsyncKeyState(VK_BACK) & 0x8000) {
			std::cout << "!!! EMERGENCY STOP !!!" << std::endl;
			// モーター1を即時停止
			std::thread thread_stop1(stop, std::ref(*motor1), "Motor1");
			thread_stop1.detach();
			// (必要であればモーター2も停止)
			std::thread thread_stop2(stop, std::ref(*motor2), "Motor2");
			thread_stop2.detach();
			// 誤動作防止のため少し待機
			sleep(500);
			continue; // 以降の処理をスキップしてループ先頭へ
		}
		// [Shift + P] -> ステップ数(g_step_count)を数値入力で変更
		if (isShiftPressed && (GetAsyncKeyState(0x50) & 0x8000)) { // 0x50 is 'P'
			sleep(300); // チャタリング防止
			// 入力関数を呼び出し (入力完了までここで待機)
			int newVal = getInputValueFromConsole("Step Count", g_step_count);
			// 有効な値なら更新
			if (newVal != -1) {
				g_step_count = newVal;
			}
		}
		// [Shift + V] -> 速度(g_step_speed)を数値入力で変更
		else if (isShiftPressed && (GetAsyncKeyState(0x56) & 0x8000)) { // 0x56 is 'V'
			sleep(300);
			int newVal = getInputValueFromConsole("Motor Speed", g_step_speed);
			if (newVal != -1) {
				// 安全のため上限・下限チェック（必要に応じて調整）
				if (newVal > 30000) newVal = 30000;
				if (newVal < 1) newVal = 1;
				g_step_speed = newVal;
			}
		}
		// ↑↑↑ ★★★ Modbusモーター制御ロジックの変更ここまで ★★★ ↑↑↑

		//焦点変更画像保存########################
		if (GetAsyncKeyState(0x38)) {	//８ボタン
			save_img_depth_flag = TRUE;
			save_img_depth = 0;
			sleep(200);
		}
		//焦点変更画像保存########################

		//if (MAG_ExpandedIMG > s_MAG_ExpandedIMG)MAG_ExpandedIMG = s_MAG_ExpandedIMG;
		//else if (MAG_ExpandedIMG < min_MAG_ExpandedIMG)MAG_ExpandedIMG = min_MAG_ExpandedIMG;
		//printf("MAG=%f\n", MAG_ExpandedIMG);
		if (GetAsyncKeyState(0x47) && GetAsyncKeyState(0x59)) { //（GとYボタンの同時押し）ガンマ補正あり
			window_states.gamma_view_flag = TRUE;
			printf("ガンマ補正モード：ON\n");
			sleep(100);
		}
		if (GetAsyncKeyState(0x47) && GetAsyncKeyState(0x4E)) { //（GとNボタンの同時押し）ガンマ補正なし
			window_states.gamma_view_flag = FALSE;
			printf("ガンマ補正モード：OFF\n");
			sleep(100);
		}

		//フェーズ移行アルゴリズム
		double height1 = -2500.0;//搬送時の卵子と同じ高さ
		double height2;//搬送時の卵子より少し下
		double height3 = -2694.5;//最下面
		double auto_dWD1 = 0.485; //搬送時の卵子と同じ高さ
		double auto_dWD3 = 0.2; //最下面
		double man1_set[3] = { 3650, -2150, -2200 };
		double man3_set[3] = { 3650, -2850, -2200 };
		double man13_cent[3] = { 3350, -2500, -2200 };

		//マイクロビーズの時
		double in_rev1 = 1.4;
		double out_rev1 = -0.6;
		double in_rev2 = 1.8;
		double out_rev2 = -0.6;

		//set_pos12d[2] = man2_cpos[2];
		//set_pos22d[2] = man4_cpos[2];
		//cent_pos12d[2] = man2_cpos[2];
		//cent_pos22d[2] = man4_cpos[2];
		//camera_to_world1(set_pos12d, set_pos13d);
		//camera_to_world2(set_pos22d, set_pos23d);
		//camera_to_world1(cent_pos12d, cent_pos13d);
		//camera_to_world2(cent_pos22d, cent_pos23d);

		std::vector<double> center{ 2500, -2500, -2500 };
		std::vector<double> set_pos1{ 2800,-1500,-2200 };
		std::vector<double> set_pos2{ 2800,-3500,-2200 };
		std::vector<double> out_pos1{ 4000,-1500,-2200 };
		std::vector<double> out_pos2{ 4000,-3500,-2200 };
		//std::vector<double> out_pos1{ set_pos13d[0] + 200,-set_pos13d[1],man2_auto_z };
		//std::vector<double> out_pos2{ set_pos23d[0] + 200,-set_pos23d[1],man4_auto_z };
		std::vector<double> wait_pos1{ 1450, -1650, -2500 };
		std::vector<double> wait_pos2{ 1450, -3350, -2500 };
		std::vector<double> cent_pos{ 3050,-2500, -2200 };
		//std::vector<double> cent_pos2{ cent_pos23d[0]+400,-cent_pos23d[1],man4_auto_z +200};
		std::vector<double> wait_pos1z{ 1450, -1450, -2200 };
		std::vector<double> wait_pos2z{ 1450, -3550, -2200 };



		//体細胞保持
		if (GetAsyncKeyState(0x60) && move_mode == 1) {
			phase_flag = 1;
			move_flag = 0; //xyのみ動くように変更
			move_mode = 0;
			//s_dWD = auto_dWD3; //焦点面を最下面に移動
			 //ピペットを最下面まで移動
			std::vector<double> movepos{ man3_xw,man3_yw,height3 };
			Movepos(hDevice_RD, s, movepos);
			man3_zw = height3;
			move_flag = 2;
			move_mode = 1;
			hDevice_move_r = hDevice_RD;
			sleep(300);
		}
		else if (phase_2_flag == 1) { //卵子の移動&卵子の回転 Iボタン

			//移動先を勝手に拡大
			phantom_flag = 2;
			//インジェクションピペット周辺の切り抜き
			point_man_x = 830; //後で決める //移動先の卵子の位置
			point_man_y = 500; //後で決める
			point_man_x_s = point_man_x;
			point_man_y_s = point_man_y;

			pointed_pixelx[pointed_count] = point_man_x;
			pointed_pixely[pointed_count] = point_man_y;

			//pointed_pixelx[pointed_count] = x;
			//pointed_pixely[pointed_count] = y;	
			pointed_mirrorx[pointed_count] = -atan((double)((point_man_x - IMG_WIDTH * MAG_IMG / 2) * MAG * resolution * frame_width / (IMG_WIDTH * MAG_IMG) + (double(center_framex2) - frame_width / 2) * resolution) / ((l2 + l3) * 1000 - z_in)) / 2 * 180 / PI;
			pointed_mirrory[pointed_count] = atan((double)((point_man_y - frame_height * double(IMG_WIDTH) * MAG_IMG / frame_width / 2) * MAG * resolution * frame_height / (frame_height * double(IMG_WIDTH) * MAG_IMG / frame_width) + (double(center_framey2) - frame_height / 2) * resolution) / ((l3) * 1000 - z_in)) / 2 * 180 / PI;
			pointed_WD[pointed_count] = derive_WD(pointed_mirrorx[pointed_count] * 2, pointed_mirrory[pointed_count] * 2);
			t_pointed_count = pointed_count;
			pointed_count++;
			if (pointed_count > max_pointed_count) pointed_count = 0;

			for (int npointed_count = 0; npointed_count < pointed_count; npointed_count++) {

				if (pointed_WD[npointed_count] > pointed_WD[pointed_count - 1]) {
					double a_line, b_line, c_line, d_line, e_line;
					a_line = pointed_pixelx[pointed_count - 1];
					b_line = pointed_pixely[pointed_count - 1];
					c_line = pointed_mirrorx[pointed_count - 1];
					d_line = pointed_mirrory[pointed_count - 1];
					e_line = pointed_WD[pointed_count - 1];
					pointed_pixelx[pointed_count - 1] = pointed_pixelx[npointed_count];
					pointed_pixely[pointed_count - 1] = pointed_pixely[npointed_count];
					pointed_mirrorx[pointed_count - 1] = pointed_mirrorx[npointed_count];
					pointed_mirrory[pointed_count - 1] = pointed_mirrory[npointed_count];
					pointed_WD[pointed_count - 1] = pointed_WD[npointed_count];
					pointed_pixelx[npointed_count] = a_line;
					pointed_pixely[npointed_count] = b_line;
					pointed_mirrorx[npointed_count] = c_line;
					pointed_mirrory[npointed_count] = d_line;
					pointed_WD[npointed_count] = e_line;
				}
			}
			center_flag = 0;
			phase_2_flag = 0;
		}
		//else if (GetAsyncKeyState(0x61)) { //徐核
		//	phase_flag = 3;
		//	move_flag = 0;
		//	move_mode = 0;
		//	s_dWD = auto_dWD1;
		//	std::vector<double> movepos3{ man3_set[0],man3_set[1],man3_set[2] }; //待機ポジ、後で決める
		//	Movepos(hDevice_RD, s, movepos3);
		//	man3_xw = man3_set[0];
		//	man3_yw = man3_set[1];
		//	man3_zw = man3_set[2];
		//	sleep(300);
		//	std::vector<double> movepos1{ man13_cent[0],man13_cent[1],man13_cent[2] }; //待機ポジ、後で決める
		//	Movepos(hDevice_RU, s, movepos1);
		//	man1_xw = man13_cent[0];
		//	man1_yw = man13_cent[1];
		//	man1_zw = man13_cent[2];
		//	man1_xw0 = man1_xw - (pPosition[0] * Kp_pha);
		//	man1_yw0 = man1_yw - (-pPosition[2] * Kp_pha);
		//	man1_zw0 = man1_zw - (pPosition[1] * Kp_pha_z);
		//	sleep(300);
		//	hDevice_move_r = hDevice_RU;
		//	move_flag = 2;
		//	move_mode = 2;
		//	printf("除核");
		//}
		//else if (GetAsyncKeyState(0x62)) { //体細胞挿入
		//	phase_flag = 4;
		//	move_flag = 0;
		//	move_mode = 0;
		//	sleep(50);
		//	//ピペットを移動
		//	std::vector<double> movepos1{ man1_set[0],man1_set[1],man1_set[2] }; //待機ポジ、後で決める
		//	Movepos(hDevice_RU, s, movepos1);
		//	man1_xw = man1_set[0];
		//	man1_yw = man1_set[1];
		//	man1_zw = man1_set[2];
		//	sleep(500);
		//	std::vector<double> movepos3{ man13_cent[0],man13_cent[1],man13_cent[2] }; //待機ポジ、後で決める
		//	Movepos(hDevice_RD, s, movepos3);
		//	man3_xw = man13_cent[0];
		//	man3_yw = man13_cent[1];
		//	man3_zw = man13_cent[2];
		//	man3_xw0 = man3_xw - (pPosition[0] * Kp_pha);
		//	man3_yw0 = man3_yw - (-pPosition[2] * Kp_pha);
		//	man3_zw0 = man3_zw - (pPosition[1] * Kp_pha_z);
		//	sleep(300);
		//	hDevice_move_r = hDevice_RD;
		//	move_flag = 2;
		//	move_mode = 1;
		//	printf("体細胞挿入");
		//}

		//体細胞保持
		if (GetAsyncKeyState(0x60) && auto_flag > 0 && move_mode == 2) {
			if (height_flag == 1) {
				move_flag = 0;
				move_mode = 0;
				Getposition(hDevice_RU, man13_store[0], man13_store[1], man13_store[2]);
				std::vector<double> movepos1{ man13_store[0],man13_store[1],man13_cent[2] };
				Movepos(hDevice_RU, s, movepos1);
				man1_xw = man13_store[0];
				man1_yw = man13_store[1];
				man1_zw = man13_cent[2];
				sleep(300);
				move_flag = 2;
				move_mode = 2;
				phase_flag = 3;
				printf("高さ変更");
				height_flag = 2;
			}
			else if (height_flag == 2) {
				move_flag = 0;
				move_mode = 0;
				Getposition(hDevice_RU, man13_store[0], man13_store[1], man13_store[2]);
				std::vector<double> movepos1{ man13_store[0],man13_store[1],-2240 };
				Movepos(hDevice_RU, s, movepos1);
				man1_xw = man13_store[0];
				man1_yw = man13_store[1];
				man1_zw = -2240;
				sleep(300);
				move_flag = 2;
				move_mode = 2;
				phase_flag = 3;
				printf("高さ変更");
				height_flag = 1;
			}
		}

		//右フットペダル Aキー(フェーズの移行)
		if (GetAsyncKeyState(0x41)) {
			if (phase_flag == 3 || phase_flag == 2) { //入れ替え
				move_flag = 0;
				move_mode = 0;
				sleep(50);
				Getposition(hDevice_RU, man13_store[0], man13_store[1], man13_store[2]);//操作中のピペットの座標を保存
				sleep(10);
				//ピペットを移動
				std::vector<double> movepos1{ man13_store[0] + 300,man13_store[1] + 350 ,man1_set[2] }; //待機ポジ
				Movepos(hDevice_RU, s, movepos1);
				man1_xw = man13_store[0] + 300;
				man1_yw = man13_store[1] + 350;
				man1_zw = man1_set[2];
				sleep(500);
				std::vector<double> movepos3{ man13_store[0],man13_store[1],man13_cent[2] + man3_auto_dz };
				Movepos(hDevice_RD, s, movepos3);
				man3_xw = man13_store[0];
				man3_yw = man13_store[1];
				man3_zw = man13_cent[2] + man3_auto_dz;
				man3_xw0 = man3_xw - (pPosition[0] * Kp_pha);
				man3_yw0 = man3_yw - (-pPosition[2] * Kp_pha);
				man3_zw0 = man3_zw - (pPosition[1] * Kp_pha_z);
				sleep(300);
				hDevice_move_r = hDevice_RD;
				move_flag = 2;
				move_mode = 1;
				phase_flag = 4;
				cout << "man3_zw = " << man13_cent[2] + man3_auto_dz << endl;
			}
			else if (phase_flag == 4) {
				move_flag = 0;
				move_mode = 0;
				//s_dWD = auto_dWD1;
				Getposition(hDevice_RD, man13_store[0], man13_store[1], man13_store[2]);
				sleep(10);
				std::vector<double> movepos3{ man13_store[0] + 300,man13_store[1] - 350,man3_set[2] }; //待機ポジ、後で決める
				Movepos(hDevice_RD, s, movepos3);
				man3_xw = man13_store[0] + 300;
				man3_yw = man13_store[1] - 350;
				man3_zw = man3_set[2];
				sleep(500);
				std::vector<double> movepos1{ man13_store[0],man13_store[1],man13_cent[2] + man1_auto_dz }; //待機ポジ、後で決める
				Movepos(hDevice_RU, s, movepos1);
				man1_xw = man13_store[0];
				man1_yw = man13_store[1];
				man1_zw = man13_cent[2] + man1_auto_dz;
				man1_xw0 = man1_xw - (pPosition[0] * Kp_pha);
				man1_yw0 = man1_yw - (-pPosition[2] * Kp_pha);
				man1_zw0 = man1_zw - (pPosition[1] * Kp_pha_z);
				sleep(300);
				hDevice_move_r = hDevice_RU;
				move_flag = 2;
				move_mode = 2;
				phase_flag = 3;
				cout << "man1_zw = " << man13_cent[2] + man1_auto_dz << endl;
			}
		}

		if (GetAsyncKeyState(0x45)) {
			auto_step = 5;
			printf("搬送終了");
			sleep(1000);
		}

		//Nボタン モータ正転(吸引)
		if (GetAsyncKeyState(0x4E)) {
			if (active_flag == 2) { //ピペット2が中央にある
				target_angle[3] += 0.05;
				//inside_count ++;
				inside_count += 0.025;
			}
			else if (active_flag == 4) { //ピペット4が中央にある
				target_angle[1] += 0.05;
				inside_count += 0.025;
				//inside_count++;
			}
			cout << "target_angle1=" << target_angle[3] << endl;
			cout << "target_angle3=" << target_angle[1] << endl;
			sleep(10);
		}

		//Mボタン モータ逆転(吐出)
		if (GetAsyncKeyState(0x4D)) {
			if (active_flag == 2) { //ピペット2が中央にある
				target_angle[3] -= 0.05;

			}
			else if (active_flag == 4) { //ピペット4が中央にある
				target_angle[1] -= 0.05;
			}
			cout << "target_angle1=" << target_angle[3] << endl;
			cout << "target_angle3=" << target_angle[1] << endl;
			sleep(10);
		}

		//Jボタン 高さ変更
		if (GetAsyncKeyState(0x4A)) {
			if (phase_flag == 2) { //高さを細胞に合わせる(除核)
				move_flag = 0;
				move_mode = 0;
				Getposition(hDevice_RU, man13_store[0], man13_store[1], man13_store[2]);
				std::vector<double> movepos1{ man13_store[0],man13_store[1],man13_cent[2] + man1_auto_dz };
				Movepos(hDevice_RU, s, movepos1);
				man1_xw = man13_store[0];
				man1_yw = man13_store[1];
				man1_zw = man13_cent[2] + man1_auto_dz;
				sleep(300);
				move_flag = 2;
				move_mode = 2;
				phase_flag = 3;
				cout << "man1_zw = " << man13_cent[2] + man1_auto_dz << endl;
			}
			else if (phase_flag == 3) { //高さを細胞に合わせる(除核)
				move_flag = 0;
				move_mode = 0;
				Getposition(hDevice_RU, man13_store[0], man13_store[1], man13_store[2]);
				std::vector<double> movepos1{ man13_store[0],man13_store[1],-2250 + man1_auto_dz };
				Movepos(hDevice_RU, s, movepos1);
				man1_xw = man13_store[0];
				man1_yw = man13_store[1];
				man1_zw = -2250 + man1_auto_dz;
				sleep(300);
				move_flag = 2;
				move_mode = 2;
				phase_flag = 2;
				cout << "man1_zw = " << man13_cent[2] + man1_auto_dz << endl;
			}
		}

		//インジェクションピペットの高さ微調整
		if (GetAsyncKeyState(0x64)) { //上に
			if (move_mode == 2) {
				man1_auto_dz += 0.3;
				move_flag = 0;
				move_mode = 0;
				Getposition(hDevice_RU, man13_store[0], man13_store[1], man13_store[2]);
				std::vector<double> movepos1{ man13_store[0],man13_store[1],man13_cent[2] + man1_auto_dz };
				Movepos(hDevice_RU, s, movepos1);
				man1_xw = man13_store[0];
				man1_yw = man13_store[1];
				man1_zw = man13_cent[2] + man1_auto_dz;
				sleep(10);
				move_flag = 2;
				move_mode = 2;
				phase_flag = 2;
				cout << "man1_auto_dz,man1_zw  =" << man1_auto_dz << "," << man13_cent[2] + man1_auto_dz << endl;
			}
			else if (move_mode == 1) {
				man3_auto_dz += 0.3;
				move_flag = 0;
				move_mode = 0;
				Getposition(hDevice_RD, man13_store[0], man13_store[1], man13_store[2]);
				std::vector<double> movepos3{ man13_store[0],man13_store[1],man13_cent[2] + man3_auto_dz };
				Movepos(hDevice_RD, s, movepos3);
				man3_xw = man13_store[0];
				man3_yw = man13_store[1];
				man3_zw = man13_cent[2] + man3_auto_dz;
				sleep(10);
				move_flag = 2;
				move_mode = 1;
				//phase_flag = 2;
				cout << "man3_auto_dz =" << man3_auto_dz << endl;
			}
		}
		if (GetAsyncKeyState(0x61)) { //上に
			if (move_mode == 2) {
				man1_auto_dz -= 0.3;
				move_flag = 0;
				move_mode = 0;
				Getposition(hDevice_RU, man13_store[0], man13_store[1], man13_store[2]);
				std::vector<double> movepos1{ man13_store[0],man13_store[1],man13_cent[2] + man1_auto_dz };
				Movepos(hDevice_RU, s, movepos1);
				man1_xw = man13_store[0];
				man1_yw = man13_store[1];
				man1_zw = man13_cent[2] + man1_auto_dz;
				sleep(10);
				move_flag = 2;
				move_mode = 2;
				phase_flag = 2;
				cout << "man1_auto_dz =" << man1_auto_dz << endl;
			}
			else if (move_mode == 1) {
				man3_auto_dz -= 0.3;
				move_flag = 0;
				move_mode = 0;
				Getposition(hDevice_RD, man13_store[0], man13_store[1], man13_store[2]);
				std::vector<double> movepos3{ man13_store[0],man13_store[1],man13_cent[2] + man3_auto_dz };
				Movepos(hDevice_RD, s, movepos3);
				man3_xw = man13_store[0];
				man3_yw = man13_store[1];
				man3_zw = man13_cent[2] + man3_auto_dz;
				sleep(10);
				move_flag = 2;
				move_mode = 1;
				//phase_flag = 2;
				cout << "man3_auto_dz =" << man3_auto_dz << endl;
			}
		}

		////Nキー+テンキー2 モータゼロ点+
		//if (GetAsyncKeyState(0x4E) && GetAsyncKeyState(0x32)) {
		//	//ex_normaly = ex_normaly + 1;
		//	//std::cout << "ex_normaly=" << ex_normaly << endl;
		//	DA_Data_zero[1] = DA_Data_zero[1] + 2;
		//	//l1 = l1 + 0.1;
		//	std::cout << "DA_Data_zero=" << DA_Data_zero[1] << endl;
		//	//act_output_DA1();
		//	zero_output_DA();
		//	sleep(50);
		//}
		////Nキー+テンキー1 モータゼロ点-
		//if (GetAsyncKeyState(0x4E) && GetAsyncKeyState(0x31)) {
		//	//ex_normaly = ex_normaly - 1;
		//	//std::cout << "ex_normaly=" << ex_normaly << endl;
		//	DA_Data_zero[1] = DA_Data_zero[1] - 2;
		//	//l1 = l1 - 0.1;
		//	std::cout << "DA_Data_zero=" << DA_Data_zero[1] << endl;
		//	zero_output_DA();
		//	//act_output_DA1();
		//	sleep(50);
		//}

		////モータの動作確認(正転) Mキー
		//if (GetAsyncKeyState(0x4D)) {
		//	//int DA_Data_auto = 32768 + 20;

		//	if (GetAsyncKeyState(0x31)) {
		//		cout << "モータ1正転" << endl;
		//		target_angle[0] +=1.0;
		//		//act_output_DA_key1(32648);
		//		//Sleep(200);
		//		get_revolution(0);
		//		//zero_output_DA1();
		//		sleep(500);
		//	}
		//	if (GetAsyncKeyState(0x32)) {
		//		cout << "モータ2正転" << endl;
		//		target_angle[1] += 1.0;
		//		//DA_Data[1] = DA_Data[1] + 500;
		//		//act_output_DA_key2(DA_Data[1]);
		//		//Sleep(200);
		//		get_revolution(1);
		//		//zero_output_DA2();
		//		sleep(500);
		//	}
		//	if (GetAsyncKeyState(0x33)) {
		//		cout << "モータ3正転" << endl;
		//		target_angle[2] += 1.0;
		//		//act_output_DA_key3(DA_Data_auto);
		//		//Sleep(200);
		//		get_revolution(2);
		//		//zero_output_DA3();
		//		sleep(500);
		//	}
		//	if (GetAsyncKeyState(0x34)) {
		//		cout << "モータ4正転" << endl;
		//		target_angle[3] += 1.0;
		//		//act_output_DA_key4(32445);
		//		//Sleep(400);
		//		get_revolution(3);
		//		//zero_output_DA4();
		//		sleep(500);
		//	}
		//	if (GetAsyncKeyState(0x35)) {
		//		cout << "モータ1逆転" << endl;
		//		target_angle[0] -= 1.0;
		//		//DA_Data[1] = DA_Data[1] - 500;
		//		//act_output_DA_key2(DA_Data[1]);
		//		//Sleep(200);
		//		get_revolution(0);
		//		//zero_output_DA2();
		//		sleep(500);
		//	}

		//	if (GetAsyncKeyState(0x36)) {
		//		cout << "モータ2逆転" << endl;
		//		target_angle[1] -= 1.0;
		//		//DA_Data[1] = DA_Data[1] - 500;
		//		//act_output_DA_key2(DA_Data[1]);
		//		//Sleep(200);
		//		get_revolution(1);
		//		//zero_output_DA2();
		//		sleep(500);
		//	}

		//	if (GetAsyncKeyState(0x37)) {
		//		cout << "モータ2逆転" << endl;
		//		target_angle[2] -= 1.0;
		//		//DA_Data[1] = DA_Data[1] - 500;
		//		//act_output_DA_key2(DA_Data[1]);
		//		//Sleep(200);
		//		get_revolution(2);
		//		//zero_output_DA2();
		//		sleep(500);
		//	}

		//	if (GetAsyncKeyState(0x38)) {
		//		cout << "モータ2逆転" << endl;
		//		target_angle[3] -= 1.0;
		//		//DA_Data[1] = DA_Data[1] - 500;
		//		//act_output_DA_key2(DA_Data[1]);
		//		//Sleep(200);
		//		get_revolution(3);
		//		//zero_output_DA2();
		//		sleep(500);
		//	}
		//}

		//injector自動化制御ループ
//キーボード"3"&スペース入力でオメガによるモーター制御切り替え　(inj_motor = 0 :制御可　inj_motor = 1 :制御停止)
		if (GetAsyncKeyState(0x33) && GetAsyncKeyState(VK_SPACE)) { //キーボード"3"入力でオメガによるモーター制御中断
			if (motor_rev == 0) {
				motor_rev = 1;
				//omega_stopangle = get_omega_angle();
				sleep(1000);
			}

			else if (motor_rev == 1) {
				motor_rev = 0;
				sleep(1000);
			}
			std::cout << "inj_motor = " << "" << motor_rev << std::endl;
		}

		//if (GetAsyncKeyState(0x65)) {
		//	if (timer_flag == 0) {
		//		QueryPerformanceCounter(&qpc_start);
		//		cout << "start_" << work_count << endl;
		//		timer_flag = 1;
		//	}
		//	else if (timer_flag == 1)
		//	{
		//		QueryPerformanceCounter(&qpc_end);
		//		if (!QueryPerformanceFrequency(&qpf)) {
		//			/* error */
		//		}
		//		cout << "end_" << work_count << endl;
		//		work_time = ((double)qpc_end.QuadPart - (double)qpc_start.QuadPart) / (double)qpf.QuadPart;
		//		data_file << work_count << "," << work_time << endl;
		//		work_count = work_count + 1;
		//		timer_flag = 0;
		//	}
		//	sleep(500);
		//}
		//cout << "target_angle=" << target_angle[0] << endl;
		//cout << "DA_Data=" << DA_Data[0] << endl;
		//cout << "revolution=" << get_revolution(0) << endl;

		//Sleep(30);
		sleep(30);

		if (end_flag == 1) break;
	}
	return 0;
}

//xbox操作
unsigned __stdcall mythread7(LPVOID lpx)
{

	int mdy;

	QueryPerformanceFrequency(&qpf);
	//マニピュレータボードのオープンと初期設定
	hDevice_RU = McsdOpen((LPCTSTR)szBoadName, 0);//ボードオープン
	hDevice_LU = McsdOpen((LPCTSTR)szBoadName, 1);//ボードオープン
	hDevice_LD = McsdOpen((LPCTSTR)szBoadName, 2);//ボードオープン
	hDevice_RD = McsdOpen((LPCTSTR)szBoadName, 3);//ボードオープン
	FirstSet(hDevice_RU);//初期設定
	FirstSet(hDevice_LU);//初期設定
	FirstSet(hDevice_RD);//初期設定
	FirstSet(hDevice_LD);//初期設定

	//特定のマニピュレータのみ原点設定
	//McsdSetCounter(hDevice_RU, 0, 0,400000);
	//McsdSetCounter(hDevice_RU, 1, 0,200000);
	//McsdSetCounter(hDevice_RU, 2, 0,800000);
	//McsdSetCounter(hDevice_LU, 0, 0, 400000);
	//McsdSetCounter(hDevice_LU, 1, 0, 200000);
	//McsdSetCounter(hDevice_LU, 2, 0, 800000);
	//McsdSetCounter(hDevice_RD, 0, 0, 400000);
	//McsdSetCounter(hDevice_RD, 1, 0, 200000);
	//McsdSetCounter(hDevice_RD, 2, 0, 800000);
	//McsdSetCounter(hDevice_LD, 0, 0, 30237);
	//McsdSetCounter(hDevice_LD, 1, 0, 493000);
	//McsdSetCounter(hDevice_LD, 2, 0, 486048);

	std::cout << hDevice_RU << std::endl;
	std::cout << hDevice_RD << std::endl;
	std::cout << hDevice_LU << std::endl;
	std::cout << hDevice_LD << std::endl;

	//初期設定
#define StickGain1 0.025
#define StickGain2 0.05
#define Highspeed 1000
#define Highspeed2 1800
#define Lowspeed 50 //250
	hDevice_move_r = hDevice_RD;
	hDevice_move_l = hDevice_LD;
	std::vector<double> speed_r{ 0,0,0 };
	std::vector<double> speed_l{ 0,0,0 };
	int direction_r[3];
	int direction_l[3];
	int lastdirection_r[3] = { 0,0,0 };
	int lastdirection_l[3] = { 0,0,0 };
	double Stick;
	Stick = StickGain1;
	LARGE_INTEGER loop_st;
	LARGE_INTEGER loop_end;
	double loop_time = 0;
	int z_speed = Lowspeed;



	//WaitForSingleObject(hEvent[7], INFINITE);


	//cout << "context=" << hlGetCurrentContext() << endl;
	//cout << "Ldevice=" << hlGetCurrentDevice() << endl;
	//cout << "Ddevice=" << hdGetCurrentDevice() << endl;
	Kp_pha_z = Kp_pha * 0.3;

	Getposition(hDevice_RU, man1_xw0, man1_yw0, man1_zw0);
	Getposition(hDevice_RD, man3_xw0, man3_yw0, man3_zw0);
	//std::cout << "man1_pos=" << man1_xw0 << "," << man1_yw0 << "," << man1_zw0 << std::endl;
	//cout << "man1_xw0" << man1_xw0 << endl;
	sleep(500);

	while (true) {
		/*QueryPerformanceCounter(&loop_st);*/
		WaitForSingleObject(hEvent[7], INFINITE);

		//cout << "Kc=" << Kc << endl;
		//cout << "Kp_pha=" << Kp_pha << endl;
		//cout << "pahntom_flag=" << phantom_flag << endl;

		//カウンタ値の初期化(2500)
		if (GetAsyncKeyState(0x35) && GetAsyncKeyState(0x43)) { //man1 5+C
			McsdSetCounter(hDevice_RU, 0, 0, 50000);
			McsdSetCounter(hDevice_RU, 1, 0, 50000);
			McsdSetCounter(hDevice_RU, 2, 0, 50000);
			sleep(1000);
		}
		else if (GetAsyncKeyState(0x36) && GetAsyncKeyState(0x43)) { //man2 6+C
			McsdSetCounter(hDevice_LU, 0, 0, 50000);
			McsdSetCounter(hDevice_LU, 1, 0, 50000);
			McsdSetCounter(hDevice_LU, 2, 0, 50000);
			sleep(1000);
		}
		else if (GetAsyncKeyState(0x37) && GetAsyncKeyState(0x43)) { //man3 7+C
			McsdSetCounter(hDevice_RD, 0, 0, 50000);
			McsdSetCounter(hDevice_RD, 1, 0, 50000);
			McsdSetCounter(hDevice_RD, 2, 0, 50000);
			sleep(1000);
		}
		else if (GetAsyncKeyState(0x38) && GetAsyncKeyState(0x43)) { //man4 8+C
			McsdSetCounter(hDevice_LD, 0, 0, 50000);
			McsdSetCounter(hDevice_LD, 1, 0, 50000);
			McsdSetCounter(hDevice_LD, 2, 0, 50000);
			sleep(1000);
		}

		//キーボードのAで操作対象変更
		if (GetAsyncKeyState(0x41) && phase_flag < 2) {
			if (move_mode == 1) {
				move_mode = 2;
				hDevice_move_r = hDevice_RU;
				hDevice_move_l = hDevice_LU;
				Getposition(hDevice_RU, man1_xw, man1_yw, man1_zw);
				man1_xw0 = man1_xw - (pPosition[0] * Kp_pha);
				man1_yw0 = man1_yw - (-pPosition[2] * Kp_pha);
				man1_zw0 = man1_zw - (pPosition[1] * Kp_pha_z);
				static_z1 = man1_zw;
			}
			else {
				move_mode = 1;
				hDevice_move_r = hDevice_RD;
				hDevice_move_l = hDevice_LD;
				Getposition(hDevice_RD, man3_xw, man3_yw, man3_zw);
				man3_xw0 = man3_xw - (pPosition[0] * Kp_pha);
				man3_yw0 = man3_yw - (-pPosition[2] * Kp_pha);
				man3_zw0 = man3_zw - (pPosition[1] * Kp_pha_z);
				static_z3 = man3_zw;
			}
			std::cout << move_mode << std::endl;
			sleep(300);
		}

		XInputGetState(0, &xbox);
		//SuspendThread(hTh[7]);

		//printf("koko1");

		speed_l[0] = 0;
		speed_l[1] = 0;
		speed_l[2] = 0;
		speed_r[0] = 0;
		speed_r[1] = 0;
		speed_r[2] = 0;

		switch (xbox.Gamepad.wButtons) {
		case XINPUT_GAMEPAD_START:
			if (z_speed == Highspeed) {
				z_speed = Highspeed2;
				Stick = StickGain2;
				printf("高速");
			}
			else {
				z_speed = Highspeed;
				Stick = StickGain1;
				printf("中速");
			}
			//z_speed = Highspeed;
			sleep(300);
			break;
		case XINPUT_GAMEPAD_BACK:
			z_speed = Lowspeed;
			break;
		case XINPUT_GAMEPAD_DPAD_UP:
			speed_l[1] = Lowspeed;
			break;
		case XINPUT_GAMEPAD_DPAD_DOWN:
			speed_l[1] = -Lowspeed;
			//Movepos(hDevice_L, s, p0);
			break;
		case XINPUT_GAMEPAD_DPAD_LEFT:
			speed_l[0] = -Lowspeed;
			//DriveStop(hDevice_L);
			//Movepos(hDevice_L, s, p1);
			//McsdDriveStop(hDevice_L, 0, 0);
			//Moveto(hDevice_L, s, p1);
			//McsdDriveStart(hDevice_L, 0, word, 100 * 20);

			//sleep(100);
			break;
		case XINPUT_GAMEPAD_DPAD_RIGHT:
			speed_l[0] = Lowspeed;
			//DriveStop(hDevice_L);
			//Movepos(hDevice_L, s, p2);
			//McsdDriveStop(hDevice_L, 0, 0);
			//Moveto(hDevice_L, s, p2);
			//McsdDriveStart(hDevice_L, 0, 0, 100 * 20);
			//sleep(100);

			break;
		case XINPUT_GAMEPAD_A:
			speed_r[1] = -Lowspeed;
			break;
		case XINPUT_GAMEPAD_B:
			speed_r[0] = Lowspeed;
			break;
		case XINPUT_GAMEPAD_X:
			speed_r[0] = -Lowspeed;
			break;
		case XINPUT_GAMEPAD_Y:
			speed_r[1] = Lowspeed;
			break;
		case XINPUT_GAMEPAD_LEFT_SHOULDER:
			speed_l[2] = z_speed;
			break;
		case XINPUT_GAMEPAD_RIGHT_SHOULDER:
			speed_r[2] = z_speed;
			break;
		default:

			if (abs(xbox.Gamepad.sThumbLX) > 3000)
			{
				speed_l[0] = xbox.Gamepad.sThumbLX * Stick;
			}
			if (abs(xbox.Gamepad.sThumbLY) > 3000)
			{
				speed_l[1] = xbox.Gamepad.sThumbLY * Stick;
			}
			if (xbox.Gamepad.bLeftTrigger > 200)
			{
				speed_l[2] = -z_speed;
			}
			if (abs(xbox.Gamepad.sThumbRX) > 3000)
			{
				speed_r[0] = xbox.Gamepad.sThumbRX * Stick;
			}
			if (abs(xbox.Gamepad.sThumbRY) > 3000)
			{
				speed_r[1] = xbox.Gamepad.sThumbRY * Stick;
			}
			if (xbox.Gamepad.bRightTrigger > 200)
			{
				speed_r[2] = -z_speed;
			}
		}

		if (speed_l[0] < 0) {
			direction_l[0] = 1;
		}
		else {
			direction_l[0] = 0;
		}
		if (speed_l[1] < 0) {
			direction_l[1] = 0;
		}
		else {
			direction_l[1] = 1;
		}
		if (speed_l[2] < 0) {
			direction_l[2] = 0;
		}
		else {
			direction_l[2] = 1;
		}
		if (speed_r[0] < 0) {
			direction_r[0] = 1;
		}
		else {
			direction_r[0] = 0;
		}
		if (speed_r[1] < 0) {
			direction_r[1] = 0;
		}
		else {
			direction_r[1] = 1;
		}
		if (speed_r[2] < 0) {
			direction_r[2] = 0;
		}
		else {
			direction_r[2] = 1;
		}

		if ((lastdirection_l[0] - direction_l[0]) != 0)	McsdDriveStop(hDevice_move_l, 0, 0);
		if ((lastdirection_l[1] - direction_l[1]) != 0)	McsdDriveStop(hDevice_move_l, 1, 0);
		if ((lastdirection_l[2] - direction_l[2]) != 0)	McsdDriveStop(hDevice_move_l, 2, 0);
		if (!g_click_move_busy) {	//クリック移動中は右側のマニピュレータに指令を出さない
			if ((lastdirection_r[0] - direction_r[0]) != 0)	McsdDriveStop(hDevice_move_r, 0, 0);
			if ((lastdirection_r[1] - direction_r[1]) != 0)	McsdDriveStop(hDevice_move_r, 1, 0);
			if ((lastdirection_r[2] - direction_r[2]) != 0)	McsdDriveStop(hDevice_move_r, 2, 0);
		}


		//past_man1_xw = man1_xw;
		//past_man1_yw = man1_yw;
		//past_man1_zw = man1_zw;

		//下限設定
		//if (man1_zw - 10 <= -2605 && speed_r[2] < 0 && move_mode==2) {
		//	speed_r[2] = 0;
		//}
		//if (man2_zw - 10 <= -2605 && speed_l[2] < 0 && move_mode == 2) {
		//	speed_l[2] = 0;
		//}
		//if (man3_zw - 10 <= -2605 && speed_r[2] < 0 && move_mode == 1) {
		//	speed_r[2] = 0;
		//}
		//if (man4_zw - 10 <= -2605 && speed_l[2] < 0 && move_mode == 1) {
		//	speed_l[2] = 0;
		//}

		//接触防止
		//if (man2_cpos[1] + 50 >= man4_cpos[1] && speed_l[1] < 0 && move_mode==2) {
		//	speed_l[1] = 0;
		//}
		//if (man2_cpos[1] + 50 >= man4_cpos[1] && speed_l[1] > 0 && move_mode == 1) {
		//	speed_l[1] = 0;
		//}

		if (auto_flag == 0) {
			Go(hDevice_move_l, speed_l);
			lastdirection_l[0] = direction_l[0];
			lastdirection_l[1] = direction_l[1];
			lastdirection_l[2] = direction_l[2];
		}

		if (use == 2) {
			if (!g_click_move_busy) Go(hDevice_move_r, speed_r);	//クリック移動中は Xbox の指令を止める（速度0の Go は DriveStop になるため）
			lastdirection_r[0] = direction_r[0];
			lastdirection_r[1] = direction_r[1];
			lastdirection_r[2] = direction_r[2];
			Getposition(hDevice_RD, man3_xw, man3_yw, man3_zw);
			//std::cout << "man3_pos=" << man3_xw << "," << man3_yw << "," << man3_zw << std::endl;
			man3_pos3D.at<double>(0, 0) = man3_xw;
			man3_pos3D.at<double>(0, 1) = -man3_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
			man3_pos3D.at<double>(0, 2) = man3_zw;//三次元時

			Getposition(hDevice_RU, man1_xw, man1_yw, man1_zw);
			//std::cout << "man1_pos=" << man1_xw << "," << man1_yw << "," << man1_zw << std::endl;
			man1_pos3D.at<double>(0, 0) = man1_xw;
			man1_pos3D.at<double>(0, 1) = -man1_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
			man1_pos3D.at<double>(0, 2) = man1_zw;//三次元時
		}
		else if (use == 1) {
			Getposition(hDevice_RD, past_man3_xw, past_man3_yw, past_man3_zw);
			//std::cout << "man3_pos=" << man3_xw << "," << man3_yw << "," << man3_zw << std::endl;
			//man3_pos3D.at<double>(0, 0) = man3_xw;
			//man3_pos3D.at<double>(0, 1) = -man3_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
			//man3_pos3D.at<double>(0, 2) = man3_zw;//三次元時

			Getposition(hDevice_RU, past_man1_xw, past_man1_yw, past_man1_zw);
			//std::cout << "man1_pos=" << past_man1_xw << "," << past_man1_yw << "," << past_man1_zw << std::endl;
			//man1_pos3D.at<double>(0, 0) = man1_xw;
			//man1_pos3D.at<double>(0, 1) = -man1_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
			//man1_pos3D.at<double>(0, 2) = man1_zw;//三次元時

			////一方のピペットの高さをもう一方に合わせる　Jボタン
			//if (GetAsyncKeyState(0x4A)) {
			//	move_flag2 = move_flag;
			//	if (GetAsyncKeyState(0x31)) { //1:1を3に合わせる
			//		move_flag = 0; //1
			//		man1_zw = man3_zw;
			//		sleep(200);
			//	}
			//	else if (GetAsyncKeyState(0x32)) { //2:2を4に合わせる
			//		move_flag = 0; //2
			//		man2_zw = man4_zw;
			//		sleep(200);
			//	}
			//	else if (GetAsyncKeyState(0x33)) { //3:3を1に合わせる
			//		move_flag = 0; //3
			//		man3_zw = man1_zw;
			//		sleep(200);
			//	}
			//	else if (GetAsyncKeyState(0x34)) { //4:4を2に合わせる
			//		move_flag = 0; //4
			//		man4_zw = man2_zw;
			//		sleep(200);
			//	}
			//	else if (GetAsyncKeyState(0x35)) { //5:1を2に合わせる
			//		move_flag = 0; //1
			//		man1_zw = man2_zw;
			//		sleep(200);
			//	}
			//	move_flag = move_flag2;
			//}
		}


		Getposition(hDevice_LU, man2_xw, man2_yw, man2_zw);
		//std::cout << "man2_pos=" << man2_xw << "," << man2_yw << "," << man2_zw << std::endl;
		man2_pos3D.at<double>(0, 0) = man2_xw;
		man2_pos3D.at<double>(0, 1) = -man2_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
		man2_pos3D.at<double>(0, 2) = man2_zw;//三次元時
		//man1_pos3D.at<double>(0, 2) = static_z;//二次元時
		Getposition(hDevice_LD, man4_xw, man4_yw, man4_zw);
		//std::cout << "man4_pos=" << man4_xw << "," << man4_yw << "," << man4_zw << std::endl;
		man4_pos3D.at<double>(0, 0) = man4_xw;
		man4_pos3D.at<double>(0, 1) = -man4_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
		man4_pos3D.at<double>(0, 2) = man4_zw;//三次元時

		//QueryPerformanceCounter(&loop_end);
		//loop_time = ((double)loop_end.QuadPart - (double)loop_st.QuadPart) / (double)qpf.QuadPart * 1000;
		//cout << "loop_time=" << loop_time << endl;

		if (end_flag == 1) {
			std::cout << "xboxクローズします" << std::endl;
			break;
		}

		////ファイルにデータを記録 20250106
		//if (file_flag == true) {
		//	data_file2 << t << "," << man1_xw << "," << man1_yw << "," << man1_zw << "," << pPosition[0] << "," << -pPosition[2] << ","
		//		<< pPosition[1] << "," << pvel_x << "," << pvel_y << "," << pvel_z << "," << pVelocity[0] << "," << -pVelocity[2] << ","
		//		<< pVelocity[1] << "," << gain << "," << m_flag << "," << man1_cpos_ex[0] << "," << pPosition2[0] << "," << -pPosition2[2] << ","
		//		<< pPosition2[1] << endl;
		//}
	}
}

//マルチスレッド8 phantomの計算&出力
unsigned __stdcall mythread8(LPVOID lpx) {
	cout << "スレッド8" << endl;
	//PhantomOpen();
	HHD hHD;
	HHLRC hHLRC;
	HDErrorInfo error;
	HLerror errorL;
	double Kp_pha_ex;
	double Kp_pha_c = 0;
	double viscos_high = 0.0;
	double viscos_low = -0.3;
	Kp_pha_ex = Kp_pha;
	LARGE_INTEGER loop_st;
	LARGE_INTEGER loop_end;
	double loop_time = 0;
	int buttonstate = 0;

	//// Initialize the default haptic device.
	hHD = hdInitDevice(HD_DEFAULT_DEVICE);
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to initialize haptic device");
		fprintf(stderr, "\nPress any key to quit.\n");
		printf("a");
		//getch();
		return 0;
	}

	// Start the servo scheduler and enable forces.
	hdEnable(HD_FORCE_OUTPUT);
	hdStartScheduler();
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to start the scheduler");
		fprintf(stderr, "\nPress any key to quit.\n");
		//getch();

		printf("a");

		return 0;
	}
	//hdScheduleSynchronous(GetDevicePosition, pPosition, HD_MIN_SCHEDULER_PRIORITY);
	printf("phantom_open");

	hHLRC = hlCreateContext(hHD);
	hlMakeCurrent(hHLRC);
	HLuint myEffect = hlGenEffects(1);
	hlBeginFrame();
	hlEffectd(HL_EFFECT_PROPERTY_GAIN, 0.0);
	hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, 0.5);
	hlStartEffect(HL_EFFECT_VISCOUS, myEffect);
	//koko
	hlEndFrame();

	errorL = hlGetError();
	cout << "error0.5=" << errorL.errorCode << endl;

	// タイマー割り込み関数の登録----------------------------------------------------
	timeBeginPeriod(1);
	gdwStartTime = timeGetTime();												// 開始時刻（windows起動時からの時間）
	gdwTimerRunTime = 0;														// タイマー実行時間[ms]
	MMRESULT timerID = timeSetEvent(1,									    // 間隔[ms]
		0,                                      // 分解能
		timerProc,                              // 割り込み関数
		(DWORD_PTR)&count2,		                    // ユーザーパラメータ
		TIME_PERIODIC | TIME_CALLBACK_FUNCTION  // 動作フラグ
	);

	printf("タイマー登録成功\n");

	if (!timerID) {
		puts("タイマー登録に失敗");
		return -2;
	}

	for (;;) {
		QueryPerformanceCounter(&loop_st);

		if (GetAsyncKeyState(0x5A)) {
			if (use == 2 && GetAsyncKeyState(0x31)) { //Z+1ボタン；phantom操作
				use = 1;
				if (move_mode == 2) {
					Getposition(hDevice_RU, man1_xw, man1_yw, man1_zw);
					hdScheduleSynchronous(GetDevicePosition, pPosition, HD_MIN_SCHEDULER_PRIORITY);
					man1_xw0 = man1_xw - (pPosition[0] * Kp_pha);
					man1_yw0 = man1_yw - (-pPosition[2] * Kp_pha);
					man1_zw0 = man1_zw - (pPosition[1] * Kp_pha_z);
					static_z1 = man1_zw;
					std::cout << "phantomA" << std::endl;
				}
				else if (move_mode == 1) {
					Getposition(hDevice_RD, man3_xw, man3_yw, man3_zw);
					hdScheduleSynchronous(GetDevicePosition, pPosition, HD_MIN_SCHEDULER_PRIORITY);
					man3_xw0 = man3_xw - (pPosition[0] * Kp_pha);
					man3_yw0 = man3_yw - (-pPosition[2] * Kp_pha);
					man3_zw0 = man3_zw - (pPosition[1] * Kp_pha_z);
					static_z3 = man3_zw;
					std::cout << "phantomB" << std::endl;
				}
			}
			else if (use == 1 && GetAsyncKeyState(0x32)) { //Z+2ボタン;xbox
				use = 2;
				std::cout << "xbox" << std::endl;
			}
			//else if (GetAsyncKeyState(0x33)) { //Z+3ボタン;半自動搬送モードの切り替え //GetAsyncKeyState(0x5A)
			//	if (auto_flag == 0) {
			//		auto_flag = 1;
			//		auto_step = 1;
			//		Getposition(hDevice_LD, man4_xw, man4_yw, man4_zw);
			//		Getposition(hDevice_LU, man2_xw, man2_yw, man2_zw);
			//		std::cout << "半自動" << auto_flag << std::endl;
			//	}
			//	else if (auto_flag == 1) {
			//		auto_flag = 0;
			//		std::cout << "手動" << auto_flag << std::endl;
			//	}
			//}
			sleep(300);
		}

		if (use == 1)
		{
			//キーボードのshiftで粘性変更
			if (GetAsyncKeyState(0x10)) {
				hlBeginFrame();
				if (phantom_flag == 1) { //視野拡張状態
					if (GetAsyncKeyState(0x30)) {//0ボタン -0.05
						viscos_low = viscos_low - 0.05;
					}
					else if (GetAsyncKeyState(0x39)) {//9ボタン +0.05
						viscos_low = viscos_low + 0.05;
					}
					hlEffectd(HL_EFFECT_PROPERTY_GAIN, viscos_low);
					hlUpdateEffect(myEffect);
					sleep(10);
				}
				else if (phantom_flag == 2) {
					if (GetAsyncKeyState(0x30)) {//0ボタン -0.05
						viscos_high = viscos_high - 0.05;
					}
					else if (GetAsyncKeyState(0x39)) {//9ボタン +0.05
						viscos_high = viscos_high + 0.05;
					}
					hlEffectd(HL_EFFECT_PROPERTY_GAIN, viscos_high);
					hlUpdateEffect(myEffect);
					sleep(10);
				}
				hlGetEffectdv(myEffect, HL_EFFECT_PROPERTY_GAIN, &gain);
				std::cout << "gain=" << gain << std::endl;
				hlEndFrame();
				sleep(500);
			}

			//通常視野にゲインを変更
			if (phantom_flag == 2)
			{
				Kc = 1246 * 0.8 / (b_framex / mag_small_img);
				//Kp_pha_c = (Kp_pha_ex / Kc) * 1.5;
				Kp_pha_c = (Kp_pha_ex / Kc) * 2.0;
				Kp_pha = Kp_pha_c; //ゲイン変更
				//hlBeginFrame();
				//hlGetEffectdv(myEffect, HL_EFFECT_PROPERTY_GAIN, &gain);
				if (gain < viscos_high - 0.01)
				{
					//hlEffectd(HL_EFFECT_PROPERTY_GAIN, viscos_high);
					//hlUpdateEffect(myEffect);
					//sleep(10);
					//errorL = hlGetError();
					//cout << "error1.3=" << errorL.errorCode << endl;
					//hlGetEffectdv(myEffect, HL_EFFECT_PROPERTY_GAIN, &gain);
					//sleep(10);
					cout << "highgain=" << gain << endl;
					cout << "Kp_pha=" << Kp_pha << endl;
					//Getposition(hDevice_RU, man1_xw0, man1_yw0, man1_zw0);
					//制御測2 0513 視野が変わった瞬間に原点を変更
				}
				//制御測2 0523
				man1_xw0 = man1_xw - (pPosition[0] * Kp_pha);
				man1_yw0 = man1_yw - (-pPosition[2] * Kp_pha);
				man1_zw0 = man1_zw - (pPosition[1] * Kp_pha_z);
				man3_xw0 = man3_xw - (pPosition[0] * Kp_pha);
				man3_yw0 = man3_yw - (-pPosition[2] * Kp_pha);
				man3_zw0 = man3_zw - (pPosition[1] * Kp_pha_z);
				phantom_flag = 0;
				//hlEndFrame();
			}
			else if (phantom_flag == 1) //視野拡張にゲインを変更
			{
				Kp_pha = Kp_pha_ex;
				//hlBeginFrame();
				//hlGetEffectdv(myEffect, HL_EFFECT_PROPERTY_GAIN, &gain);
				if (gain > viscos_low + 0.01)
				{
					//hlEffectd(HL_EFFECT_PROPERTY_GAIN, viscos_low);
					//hlUpdateEffect(myEffect);
					//sleep(10);
					//errorL = hlGetError();
					//cout << "error2.2=" << errorL.errorCode << endl;
					hlGetEffectdv(myEffect, HL_EFFECT_PROPERTY_GAIN, &gain);
					sleep(10);
					cout << "lowgain=" << gain << endl;
					cout << "Kp_pha=" << Kp_pha << endl;
					//Getposition(hDevice_RU, man1_xw0, man1_yw0, man1_zw0);
				}
				//制御測2 0523
				man1_xw0 = man1_xw - (pPosition[0] * Kp_pha);
				man1_yw0 = man1_yw - (-pPosition[2] * Kp_pha);
				man1_zw0 = man1_zw - (pPosition[1] * Kp_pha_z);
				man3_xw0 = man3_xw - (pPosition[0] * Kp_pha);
				man3_yw0 = man3_yw - (-pPosition[2] * Kp_pha);
				man3_zw0 = man3_zw - (pPosition[1] * Kp_pha_z);
				//hlEndFrame();
				phantom_flag = 0;
			}

			////スリットの位置を力覚提示
			//if (inside_flag == 6) {
			//	//力覚計算
			//	double Kf = 0.01;
			//	double force_out_x = Kf * (slit_xc - man1_cpos_c[0]);
			//	double force_out_y = Kf * (slit_yc - man1_cpos_c[1]);

			//	if (force_out_x > 2.0) {
			//		force_out_x = 2.0;
			//	}
			//	if (force_out_y > 2.0) {
			//		force_out_y = 2.0;
			//	}
			//	if (force_out_x < -2.0) {
			//		force_out_x = -2.0;
			//	}
			//	if (force_out_y < -2.0) {
			//		force_out_y = -2.0;
			//	}
			//	hduVector3Dd force_out(force_out_x, 0, force_out_y);
			//	hdScheduleSynchronous(SetDeviceForce, force_out, HD_MIN_SCHEDULER_PRIORITY);
			//	//cout << "force = " << force_out_x << "," << force_out_y << endl;
			//}

			//Jボタン 軸固定フラグ //0829 テンキーに変更
			//if (GetAsyncKeyState(0x4A)) {
			//	if (GetAsyncKeyState(0x30)) move_flag = 0; //0
			//	else if (GetAsyncKeyState(0x31)) move_flag = 1; //1
			//	else if (GetAsyncKeyState(0x32)) move_flag = 2; //2
			//	else if (GetAsyncKeyState(0x33)) move_flag = 3; //3
			//	sleep(100);
			//}
			if (GetAsyncKeyState(0x65)) {
				move_flag = 0; //5
				sleep(100);
			}
			else if (GetAsyncKeyState(0x66)) {
				move_flag = 1; //6
				sleep(100);
			}
			else if (GetAsyncKeyState(0x67)) {
				move_flag = 2; //7
				sleep(100);
			}
			else if (GetAsyncKeyState(0x68)) {
				move_flag = 3; //8
				sleep(100);
			}

			hdScheduleSynchronous(GetDevicePosition, pPosition, HD_MIN_SCHEDULER_PRIORITY);
			hdGetDoublev(HD_CURRENT_VELOCITY, pVelocity);
			dt = t - tp;
			tp = t;
			//cout << "t=" << t << endl;
			//if (GetAsyncKeyState(0x4B)) std::cout << "pPosition " << pPosition << std::endl;
			// 
			// ファントムの速度導出
			//pvel_x = 8000;
			//pvel_y = 8000;
			//pvel_z = 8000;

			//ローパスフィルタ
			pPosition2[0] = s_time * omega * (pPosition_past[0] + pPosition[0]) / (2 + s_time * omega) + (2 - s_time * omega) * pPosition2[0] / (2 + s_time * omega);
			pPosition2[1] = s_time * omega * (pPosition_past[1] + pPosition[1]) / (2 + s_time * omega) + (2 - s_time * omega) * pPosition2[1] / (2 + s_time * omega);
			pPosition2[2] = s_time * omega * (pPosition_past[2] + pPosition[2]) / (2 + s_time * omega) + (2 - s_time * omega) * pPosition2[2] / (2 + s_time * omega);

			hdScheduleSynchronous(GetButtonState, &buttonstate, HD_MIN_SCHEDULER_PRIORITY);

			if (move_mode == 1) { //下のピペットが動く
				//モードごとに動く軸が変化
				if (move_flag == 0 || GetAsyncKeyState(0x4B) || buttonstate == 1) {
					man3_xw0 = man3_xw - (pPosition2[0] * Kp_pha);
					man3_yw0 = man3_yw - (-pPosition2[2] * Kp_pha);
					man3_zw0 = man3_zw - (pPosition2[1] * Kp_pha_z);
				}
				else if (GetAsyncKeyState(0x4C)) { //Lボタン：z軸のみ動く
					man3_xw0 = man3_xw - (pPosition2[0] * Kp_pha);
					man3_yw0 = man3_yw - (-pPosition2[2] * Kp_pha);
					man3_zw = pPosition2[1] * Kp_pha_z + man3_zw0; //視野拡張時　高さのゲインが大きすぎるので半分に 0606
				}
				else if (move_flag == 2) {
					man3_xw = pPosition2[0] * Kp_pha + man3_xw0;
					man3_yw = -pPosition2[2] * Kp_pha + man3_yw0;
					man3_zw0 = man3_zw - (pPosition2[1] * Kp_pha_z); //視野拡張時
				}
				else if (move_flag == 1) {
					man3_xw = pPosition2[0] * Kp_pha + man3_xw0;
					man3_yw0 = man3_yw - (-pPosition2[2] * Kp_pha);
					man3_zw0 = man3_zw - (pPosition2[1] * Kp_pha_z); //視野拡張時
				}
				else if (move_flag == 3) {
					man3_xw = pPosition2[0] * Kp_pha + man3_xw0;
					man3_yw = -pPosition2[2] * Kp_pha + man3_yw0;
					man3_zw = pPosition2[1] * Kp_pha_z + man3_zw0; //視野拡張時　高さのゲインが大きすぎるので半分に 0606
				}


				//下限設定
				//if (man3_zw <= -2900) {
				//	man3_zw = -2900;
				//}

				//接触防止
				//if (man2_cpos[1] + 50 >= man4_cpos[1] && speed_l[1] < 0 && move_mode==2) {
				//	speed_l[1] = 0;
				//}
				//if (man2_cpos[1] + 50 >= man4_cpos[1] && speed_l[1] > 0 && move_mode == 1) {
				//	speed_l[1] = 0;
				//}


				//if (phantom_flag == 1) { //視野拡張→高さが変わる
				//	move_man3_pos = { man3_xw, man3_yw, man3_zw };//二次元時
				//	man3_pos3D.at<double>(0, 0) = man3_xw;
				//	man3_pos3D.at<double>(0, 1) = -man3_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
				//	man3_pos3D.at<double>(0, 2) = man3_zw;//三次元時
				//}
				//else if (phantom_flag == 2) {
				//	move_man3_pos = { man3_xw, man3_yw, man3_zw };//二次元時
				//	man3_pos3D.at<double>(0, 0) = man3_xw;
				//	man3_pos3D.at<double>(0, 1) = -man3_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
				//	man3_pos3D.at<double>(0, 2) = man3_zw;//三次元時
				//}

				//Movepos(hDevice_move_r, vel_phantom, move_man3_pos);
				//std::cout << "move_man3_pos=" << move_man3_pos[0] <<"," << move_man3_pos[1] << "," << move_man3_pos[2] << std::endl;
			}
			else if (move_mode == 2) { //上のピペットが動く
				//モードごとに動く軸が変化
				if (move_flag == 0 || GetAsyncKeyState(0x4B) || buttonstate == 1) {
					man1_xw0 = man1_xw - (pPosition2[0] * Kp_pha);
					man1_yw0 = man1_yw - (-pPosition2[2] * Kp_pha);
					man1_zw0 = man1_zw - (pPosition2[1] * Kp_pha_z);
				}
				else if (GetAsyncKeyState(0x4C)) { //Lボタン：z軸のみ動く
					man1_xw0 = man1_xw - (pPosition2[0] * Kp_pha);
					man1_yw0 = man1_yw - (-pPosition2[2] * Kp_pha);
					man1_zw = pPosition2[1] * Kp_pha_z + man1_zw0; //視野拡張時　高さのゲインが大きすぎるので半分に 0606
				}
				else if (move_flag == 2) {
					man1_xw = pPosition2[0] * Kp_pha + man1_xw0;
					man1_yw = -pPosition2[2] * Kp_pha + man1_yw0;
					man1_zw0 = man1_zw - (pPosition2[1] * Kp_pha_z); //視野拡張時
				}
				else if (move_flag == 1) {
					man1_xw = pPosition2[0] * Kp_pha + man1_xw0;
					man1_yw0 = man1_yw - (-pPosition2[2] * Kp_pha);
					man1_zw0 = man1_zw - (pPosition2[1] * Kp_pha_z); //視野拡張時
				}
				else if (move_flag == 3) {
					man1_xw = pPosition2[0] * Kp_pha + man1_xw0;
					man1_yw = -pPosition2[2] * Kp_pha + man1_yw0;
					man1_zw = pPosition2[1] * Kp_pha_z + man1_zw0; //視野拡張時　高さのゲインが大きすぎるので半分に 0606
				}

				//下限設定
				//if (man1_zw <= -2900) {
				//	man1_zw = -2900;
				//}

				//if (phantom_flag == 1) { //視野拡張→高さが変わる
				//	//move_man1_pos = { man1_xw, man1_yw, man1_zw };//二次元時
				//	//man1_pos3D.at<double>(0, 0) = man1_xw;
				//	//man1_pos3D.at<double>(0, 1) = -man1_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
				//	//man1_pos3D.at<double>(0, 2) = man1_zw;//三次元時
				//}
				//else if (phantom_flag == 2) {
				//	//move_man1_pos = { man1_xw, man1_yw, static_z1 };//二次元時
				//	move_man1_pos = { man1_xw, man1_yw, man1_zw };
				//	man1_pos3D.at<double>(0, 0) = man1_xw;
				//	man1_pos3D.at<double>(0, 1) = -man1_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
				//	//man1_pos3D.at<double>(0, 2) = static_z1;//三次元時
				//	man1_pos3D.at<double>(0, 2) = man1_zw;
				//}
				//pvel_x = abs((man1_xw - past_man1_xw) * 2 / dt);
				//pvel_y = abs((man1_yw - past_man1_yw) * 2 / dt);
				//pvel_z = abs((man1_zw - past_man1_zw) * 2 / dt);
				//if (pvel_x > 5000) {
				//	pvel_x = 5000;
				//}
				//if (pvel_y > 5000) {
				//	pvel_y = 5000;
				//}
				//if (pvel_z > 5000) {
				//	pvel_z = 5000;
				//}
				//vel_phantom = { pvel_x, pvel_y, pvel_z };
				//Movepos(hDevice_move_r, vel_phantom, move_man1_pos);
				//std::cout << "move_man1_pos=" << move_man1_pos[0] << "," << move_man1_pos[1] << "," << move_man1_pos[2] << std::endl;
			}

			//cout << "vel= " << pvel_x << "," << pvel_y << "," << pvel_z << endl;

			//past_man1_xw = man1_xw;
			//past_man1_yw = man1_yw;
			//past_man1_zw = man1_zw;
			//past_man3_xw = man3_xw;
			//past_man3_yw = man3_yw;
			//past_man3_zw = man3_zw;
			pPosition_past[0] = pPosition[0];
			pPosition_past[1] = pPosition[1];
			pPosition_past[2] = pPosition[2];

			//cout << "man1_w" << man1_xw << "  " << -man1_yw << endl;
			//cout << "man1_w0" << man1_xw0 << "  " << -man1_yw0 << endl;

			//man1の３次元座標をカメラ座標系に変換
			//if (man1_flag == point_number + 2) {
			//	projectPoints(man1_pos3D, rvec1, tvec1, K, distortion, man1_pos2D);

			//	man1_cpos[0] = man1_pos2D.at<double>(0, 0) + man1_dx;
			//	man1_cpos[1] = man1_pos2D.at<double>(0, 1) + man1_dy;
			//	man1_zc = rmat1.at<double>(0, 2) * man1_pos3D.at<double>(0, 0) + rmat1.at<double>(1, 2) * man1_pos3D.at<double>(0, 1) + rmat1.at<double>(2, 2) * man1_pos3D.at<double>(0, 2) + tvec1.at<double>(2, 0);
			//	man1_cpos[2] = (man1_zc - man1_zc0 + (man1_s_dWD2 - s_dWD_0) * 1000) / pixelpitch * magnification;
			//	//cout << "man1_cpos"<<man1_cpos[0] << "  " << man1_cpos[1] << endl;
			//}

			//if (pre1_man1_cpos[0] != man1_cpos[0] || pre1_man1_cpos[1] != man1_cpos[1]) {
			//	man1_opt_flag = 1;
			//}
			//else {
			//	//man1_opt_flag = 0;
			//	man1_opt_flag = 1;
			//}

			//pre1_man1_cpos[0] = man1_cpos[0];		//１ループ前のデータを保存
			//pre1_man1_cpos[1] = man1_cpos[1];		//１ループ前のデータを保存
			//pre1_man1_cpos[2] = man1_cpos[2];		//１ループ前のデータを保存


			//sleep(2);

			//		//インジェクション時に力を返す
			//		//if (man1_MorR == 1 && NOC == 1 && ReturnForce1 == true) {
			//		if (man1_MorR == 1 && ReturnForce1 == true) {
			//			//double cell_to_man1 = sqrt(pow(man1_cpos[0] - center_x[0], 2) + pow(man1_cpos[1] - center_y[0], 2) + pow(man1_cpos[2] - center_z[0], 2));//man1の先端から操作対象の中心までの距離(pixel)
			//			double cell_to_man1 = sqrt(pow(man1_cpos[0] - (man2_cpos[0] + opt_r), 2) + pow(man1_cpos[1] - man2_cpos[1], 2));//man1の先端から操作対象の中心までの距離(pixel)
			//			//cout << cell_to_man1 <<endl;
			//			//if (cell_to_man1 < radius[0]) {
			//			if (cell_to_man1 < opt_r) {
			//				//wd_emb = (radius[0] - cell_to_man1) / pixelpitch * magnification;
			//				//wd_emb = (center_x[0] + radius[0] - man1_cpos[0]) / pixelpitch * magnification;
			//				//wd_emb = (center_x[0] + radius[0] - man1_cpos[0]) * pixelpitch / magnification;//円検出できたときに使うやつ
			//				wd_emb = ((man2_cpos[0] + opt_r) + opt_r - man1_cpos[0]) * pixelpitch / magnification;//円検出できないときに使うやつ
			//				//cout << "wd_emb" << endl;
			//			}
			//			else {
			//				wd_emb = 0.0;
			//			}
			//			pre_force_inj_in = force_in;
			//			force_in = 2.0 * PI * E_emb * h_emb * pow(wd_emb, 3) * (3 - 4 * ca2 + ca2 * ca2 + 2 * log(ca2)) / (a_emb * a_emb * (1 - p_emb) * (1 - ca2) * pow(1 - ca2 + log(ca2), 3));
			//
			//			//ローパスフィルタ
			//			force_inj_in2 = s_time2 * omega2 * (pre_force_inj_in + force_in) / (2 + s_time2 * omega2) + (2 - s_time2 * omega2) * force_inj_in2 / (2 + s_time2 * omega2);
			//
			//			hduVector3Dd forceDirection(1.0, 0.0, 0.0);
			//			//hduVector3Dd forceDirection(0.76604, 0.64279, 0.0);//インジェクションピペットの軸方向に力を返す（４０°）
			//			//hduVector3Dd force2 = force_inj * k_force * forceDirection;
			//			force_out = force_inj_in2 * k_inj * forceDirection;
			//			//cout << "force_out" << force_out << endl;
			//			hdScheduleSynchronous(SetDeviceForce, force_out, HD_MIN_SCHEDULER_PRIORITY);
			//		}
			//
			//		//胚回転時に力を返す
			//		//cell_x_roll = man2_cpos[0] + Man2ToCell_x;
			//		//cell_y_roll = man2_cpos[1] + Man2ToCell_y;
			//		//cell_z_roll = man2_cpos[2] + Man2ToCell_z;
			//		//cell_to_man1_tip = sqrt(pow(man1_cpos[0] - cell_x_roll, 2) + pow(man1_cpos[1] - cell_y_roll, 2) + pow(man1_cpos[2] - cell_z_roll, 2));//man1の先端から操作対象の中心までの距離(pixel)
			//		//cell_move = sqrt(pow(center_x[0] - cell_x_roll, 2) + pow(center_y[0] - cell_y_roll, 2) + pow(center_z[0] - cell_z_roll, 2));
			//		////double cell_to_man1 = sqrt(pow(man1_cpos[0] - center_x[0], 2) + pow(man1_cpos[1] - center_y[0], 2));//man1の先端から操作対象の中心までの距離(pixel) center_zの代わりにman2_cpos[2]を使用
			//		////if (man1_MorR == 1 && NOC == 1 && ReturnForce2 == true) {
			//		//if (man1_MorR == 1 && NOC == 1 && ReturnForce2 == true && cell_move < 100) {
			//		//	//if (man1_MorR == 1 && NOC == 1 && ReturnForce2 == true && cell_to_man1_tip <= radius[0]) {
			//		//		//hduVector3Dd CtoC0(0, 0, cell_y_roll - center_y[0]);
			//		//	hduVector3Dd CtoC0(0, cell_z_roll - center_z[0], cell_y_roll - center_y[0]);
			//		//	pre_force_roll = force_roll;
			//		//	force_roll = k_roll * CtoC0;
			//
			//		//	//ローパスフィルタ
			//		//	force_roll2 = s_time2 * omega2 * (pre_force_roll + force_roll) / (2 + s_time2 * omega2) + (2 - s_time2 * omega2) * force_roll2 / (2 + s_time2 * omega2);
			//
			//		//	hdSetDoublev(HD_CURRENT_FORCE, force_roll2);
			//		//	//printf("(x,y)=(%f,%f)\n", cell_x_roll - center_x[0], cell_y_roll - center_y[0]);
			//		//	//cout << force_roll << endl;
			//		//}
			//		//else {
			//		//	hduVector3Dd no_force(0, 0, 0);
			//		//	//hdScheduleSynchronous(SetDeviceForce, no_force, HD_MIN_SCHEDULER_PRIORITY);
			//		//}
			//
			//		dx_cell = (man1_cpos[0] - center_x[0] - radius[0]) * pixelpitch / magnification;
			//		dy_cell = (man1_cpos[1] - center_y[0]) * pixelpitch / magnification;
			//		dz_cell = (man1_cpos[2] - center_z[0]) * pixelpitch / magnification;
			//		dr_cell = sqrt(dx_cell * dx_cell + dy_cell * dy_cell + dz_cell * dz_cell);
			//
			//		if (end_flag == 1) break;
			//		QueryPerformanceCounter(&mythread5_end);
			//		mythread5_time = ((double)mythread5_end.QuadPart - (double)mythread5_start.QuadPart) / (double)qpf.QuadPart * 1000;
			//		//printf("Time : %f\n", mythread5_time);
			//		all_time = all_time + mythread5_time / 1000;
			//
			//		//ファイルにデータを記録
			//		if (file_flag == true) {
			//			data_file << mythread5_time << "," << all_time << "," << center_x[0] << "," << center_y[0] << "," << center_z[0] << "," << radius[0] << ","
			//				<< man1_pos3D.at<double>(0, 0) << "," << man1_pos3D.at<double>(0, 1) << "," << man1_pos3D.at<double>(0, 2) << "," << man2_pos3D.at<double>(0, 0) << "," << man2_pos3D.at<double>(0, 1) << "," << man2_pos3D.at<double>(0, 2) << ","
			//				<< man1_cpos[0] << "," << man1_cpos[1] << "," << man1_cpos[2] << "," << man2_cpos[0] << "," << man2_cpos[1] << "," << man2_cpos[2] << "," << force_inj_in2 << "," << force_inj_in2 * k_inj << ","
			//				<< dx_cell << "," << dy_cell << "," << dz_cell << "," << dr_cell << ","
			//				<< force_roll2[2] << "," << force_roll2[1] << "," << cell_y_roll - center_y[0] << "," << cell_z_roll - center_z[0] << endl;
			//		}
			//
			//		//データ記録用のファイルを閉じる（スペース＆Cボタン）
			//		if (file_flag == true && GetAsyncKeyState(VK_SPACE) && GetAsyncKeyState(0x43)) {
			//			data_file.close();
			//			file_flag = false;
			//			printf("Data%d close\n", file_number);
			//			file_number = file_number + 1;
			//			sleep(1000);
			//		}
			//
			QueryPerformanceCounter(&loop_end);
			//loop_time = ((double)loop_end.QuadPart - (double)loop_st.QuadPart) / (double)qpf.QuadPart * 1000;
			//cout << "loop_time=" << loop_time << endl;

			HDErrorInfo error;
			if (HD_DEVICE_ERROR(error = hdGetError()))
			{
				hduPrintError(stderr, &error, "Error during main scheduler callback\n");

				if (hduIsSchedulerError(&error))
				{
					return HD_CALLBACK_DONE;
				}
			}
			if (end_flag == 1) break;


		}
		//Getposition(hDevice_RU, man1_xw, man1_yw, man1_zw);
		man1_pos3D.at<double>(0, 0) = man1_xw;
		man1_pos3D.at<double>(0, 1) = -man1_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
		man1_pos3D.at<double>(0, 2) = man1_zw;//三次元時
		man3_pos3D.at<double>(0, 0) = man3_xw;
		man3_pos3D.at<double>(0, 1) = -man3_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
		man3_pos3D.at<double>(0, 2) = man3_zw;//三次元時
		if (use == 3) {

		}

		//sleep(5);

		if (end_flag == 1) break;
	}
	hlBeginFrame();
	hlStopEffect(myEffect);
	hlEndFrame();
	PhantomClose();
	return 0;
}

//スレッド9 phantomの出力
unsigned __stdcall mythread9(LPVOID lpx) {
	LARGE_INTEGER loop_st;
	LARGE_INTEGER loop_end;
	double loop_time = 0;

	for (;;) {
		QueryPerformanceCounter(&loop_st);

		if (use == 1 && !g_click_move_busy) {	//クリック移動中は Phantom の指令を出さない

			if (move_mode == 1) {
				pvel_x = abs((man3_xw - past_man3_xw) * 11.0);
				pvel_y = abs((man3_yw - past_man3_yw) * 11.0);
				pvel_z = abs((man3_zw - past_man3_zw) * 11.0);
				if (pvel_x > 5000) {
					pvel_x = 5000;
				}
				if (pvel_y > 5000) {
					pvel_y = 5000;
				}
				if (pvel_z > 5000) {
					pvel_z = 5000;
				}

				//接触防止
				if ((man2_yw - 100 <= man3_yw && man3_yw <= man2_yw + 100 && man2_xw - 30 >= past_man3_xw && (man3_xw - past_man3_xw) < 0) || (man4_yw - 100 <= man3_yw && man3_yw <= man4_yw + 100 && man4_xw - 30 >= past_man3_xw && (man3_xw - past_man3_xw) < 0)) {
					pvel_x = 0;
					man1_xw0 = man1_xw - (pPosition2[0] * Kp_pha);
				}

				vel_phantom = { pvel_x, pvel_y, pvel_z };
				move_man3_pos = { man3_xw, man3_yw, man3_zw };//二次元時
				man3_pos3D.at<double>(0, 0) = man3_xw;
				man3_pos3D.at<double>(0, 1) = -man3_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
				man3_pos3D.at<double>(0, 2) = man3_zw;//三次元時
				Movepos(hDevice_move_r, vel_phantom, move_man3_pos);
				//std::cout << "man3_pos=" << man3_xw << "," << man3_yw << "," << man3_zw << std::endl;
				//past_man3_xw = man3_xw;
				//past_man3_yw = man3_yw;
				//past_man3_zw = man3_zw;
			}
			else if (move_mode == 2) {
				pvel_x = abs((man1_xw - past_man1_xw) * 11.0);
				pvel_y = abs((man1_yw - past_man1_yw) * 11.0);
				pvel_z = abs((man1_zw - past_man1_zw) * 11.0);
				//pvel_x = 500;
				//pvel_y = 500;
				//pvel_z = 500;
				if (pvel_x > 5000) {
					pvel_x = 5000;
				}
				if (pvel_y > 5000) {
					pvel_y = 5000;
				}
				if (pvel_z > 5000) {
					pvel_z = 5000;
				}

				//接触防止
				if ((man2_yw - 100 <= man1_yw && man1_yw <= man2_yw + 100 && man2_xw >= past_man1_xw && (man1_xw - past_man1_xw) < 0) || (man4_yw - 100 <= man1_yw && man1_yw <= man4_yw + 100 && man4_xw >= past_man1_xw && (man1_xw - past_man1_xw) < 0)) {
					pvel_x = 0;
				}
				//cout << "man1_cpos=" << man1_cpos[0] << "," << man1_cpos[1] << endl;
				//cout << "man2_cpos=" << man2_cpos[0] << "," << man2_cpos[1] << endl;

				vel_phantom = { pvel_x, pvel_y, pvel_z };
				//out << "vel=" << pvel_x << "," << pvel_y << "," << pvel_z << endl;
				move_man1_pos = { man1_xw, man1_yw, man1_zw };//二次元時
				man1_pos3D.at<double>(0, 0) = man1_xw;
				man1_pos3D.at<double>(0, 1) = -man1_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
				man1_pos3D.at<double>(0, 2) = man1_zw;//三次元時
				//std::cout << "man1_pos=" << man1_xw << "," << man1_yw << "," << man1_zw << std::endl;
				//std::cout << "past_man1_pos=" << past_man1_xw << "," << past_man1_yw << "," << past_man1_zw << std::endl;
				Movepos(hDevice_move_r, vel_phantom, move_man1_pos);
				//past_man1_xw = man1_xw;
				//past_man1_yw = man1_yw;
				//past_man1_zw = man1_zw;
			}
		}
		sleep(1);

		QueryPerformanceCounter(&loop_end);
		//loop_time = ((double)loop_end.QuadPart - (double)loop_st.QuadPart) / (double)qpf.QuadPart * 1000;
		//cout << "1/loop_time=" << 1/loop_time << endl;

		if (end_flag == 1) break;

	}
	return 0;
}

//constexpr int64_t numChannels = 3;
////constexpr int64_t width = 256;
//constexpr int64_t width = 80;
//constexpr int64_t trimmed_width = 320;
//constexpr int64_t trimmed_height = 320;
////constexpr int64_t height = 256;
//constexpr int64_t height = 80;
//constexpr int64_t numClasses = 103;
//constexpr int64_t numInputElements = numChannels * height * width;
//constexpr int64_t numOutputElements = height * width * numClasses;
//int centerX=244;
//int centerY=164;
//int centerX2 = (centerX * trimmed_width / 648) - width / 2;
//int centerY2 = (centerY * trimmed_width / 488) - height / 2;
//int moX = 0;
//int moY = 0;
//int nonzero2;
//float radius_2;
//float uncertainty;
//int trimmed_area;
//int circle_area;
//Mat detect_image;
//cv::Mat mask2 = cv::Mat::zeros(648, 488, CV_8UC1); // マスク画像
//
////入力画像を前処理する関数
//std::vector<float> loadImage(const cv::Mat& view2)
//{
//	auto image = view2.clone();
//	//auto image2 = Mat(image, Rect(18, 18, 64, 64));
//	//cout << "centerX" << centerX << endl;
//	//cout << "centerY" << centerY << endl;
//	/*int centerX2 = (centerX * trimmed_width / 648) - width / 2;
//	int centerY2 = (centerY * trimmed_height / 488) - height / 2;
//	if (centerX2 < 0) {
//		centerX2 = 0;
//	}
//	if (centerX2 > width) {
//		centerX2 = width;
//	}
//	if (centerY2 < 0) {
//		centerY2 = 0;
//	}
//	if (centerY2 > height) {
//		centerY2 = height;
//	}*/
//	// OpenCVはBGRの順で読み込むのでRGBに変換
//
//	cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
//
//	centerX = man2_cpos_c[0] - 5;
//	centerY = man2_cpos_c[1] - 160;
//
//	if (centerX < 0) {
//		centerX = 0;
//	}
//	if (centerX + trimmed_width > image.cols) {
//		centerX = image.cols - trimmed_width;
//	}
//	if (centerY < 0) {
//		centerY = 0;
//	}
//	if (centerY + trimmed_width > image.rows) {
//		centerY = image.rows - trimmed_width;
//	}
//	
//	//flip(image, image, 0);
//	cout << "man2_cpos_c=" << centerX << "," << centerY << std::endl;
//	image = Mat(image, Rect(centerX, centerY, trimmed_width, trimmed_height)).clone();
//
//	imwrite("resize1.png", image);
//	cv::Mat image2;
//	
//	//cv::resize(image, image2, cv::Size(), 80 / image.cols, 80 / image.rows);
//	cv::resize(image, image2, cv::Size(), 0.25, 0.25);
//	//imshow("trimmed_img", image2);
//	/*imshow("trimmed_image", image2);
//	waitKey(0);*/
//	//cout << image << std::endl;
//	cout << image.size() << std::endl;
//	cout << image2.size() << std::endl;
//	//imshow("image", image);
//
//	// 1次元に変更
//	image2 = image2.reshape(1, 1);
//
//	// [0, 255]のuint_8から[0, 1]のfloatに変換
//	std::vector<float> vec;
//	image2.convertTo(vec, CV_32FC1, 1. / 255);
//
//	// HWC -> CHW
//	std::vector<float> output;
//	for (size_t ch = 0; ch < 3; ++ch) {
//		for (size_t i = ch; i < vec.size(); i += 3) {
//			output.emplace_back(vec[i]);
//		}
//	}
//	return output;
//}
//
//void initialize_parameters() {
//	//MAG_W = 1.0;		//値の初期化
//	//MAG_level = 1;		//値の初期化
//	//tmp_MAG_level = 1;	//値の初期化
//	//start_mirror = 0;
//	end_flag = 0;
//	//window_states.switch_window_state = 0;
//}
//
//void transparency() {
//	//モデルの入力
//	const std::wstring modelFile = L"train_opset17_117_4to103_add_size80_4layer_005.onnx";
//
//	Ort::Env env;
//	Ort::SessionOptions sessionOptions;
//	sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_DISABLE_ALL);
//	Ort::Session session(nullptr);
//
//	try {
//		session = Ort::Session(env, modelFile.data(), sessionOptions);
//	}
//	catch (Ort::Exception& e) {
//		std::cout << e.what() << std::endl;
//		return;
//	}
//
//	// 入出力のshape
//	const std::vector<int64_t> inputShape = { 1, numChannels, height, width };
//	const std::vector<int64_t> outputShape = { 1, numClasses, height, width };
//
//	// 入出力用の配列
//	std::vector<float> input(numInputElements);
//	std::vector<float> results1(numOutputElements);
//	std::vector<float> results2(numOutputElements);
//	std::vector<float> results3(numOutputElements);
//	std::vector<float> results4(numOutputElements);
//	std::vector<float> results5(numOutputElements);
//	std::vector<float> results6(numOutputElements);
//	std::vector<float> results7(numOutputElements);
//	std::vector<float> results8(numOutputElements);
//	std::vector<float> results9(numOutputElements);
//	std::vector<float> results10(numOutputElements);
//
//	// 入出力用のTensor
//	// 内部で配列の先頭ポインタを保持する 
//	//ここでとまってる
//	auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
//	auto inputTensor = Ort::Value::CreateTensor<float>(memory_info, input.data(), input.size(), inputShape.data(), inputShape.size());
//	auto outputTensor1 = Ort::Value::CreateTensor<float>(memory_info, results1.data(), results1.size(), outputShape.data(), outputShape.size());
//	//auto outputTensor2 = Ort::Value::CreateTensor<float>(memory_info, results2.data(), results2.size(), outputShape.data(), outputShape.size());
//	//auto outputTensor3 = Ort::Value::CreateTensor<float>(memory_info, results3.data(), results3.size(), outputShape.data(), outputShape.size());
//	//auto outputTensor4 = Ort::Value::CreateTensor<float>(memory_info, results4.data(), results4.size(), outputShape.data(), outputShape.size());
//	//auto outputTensor5 = Ort::Value::CreateTensor<float>(memory_info, results5.data(), results5.size(), outputShape.data(), outputShape.size());
//	//auto outputTensor6 = Ort::Value::CreateTensor<float>(memory_info, results6.data(), results6.size(), outputShape.data(), outputShape.size());
//	//auto outputTensor7 = Ort::Value::CreateTensor<float>(memory_info, results7.data(), results7.size(), outputShape.data(), outputShape.size());
//	//auto outputTensor8 = Ort::Value::CreateTensor<float>(memory_info, results8.data(), results8.size(), outputShape.data(), outputShape.size());
//	//auto outputTensor9 = Ort::Value::CreateTensor<float>(memory_info, results9.data(), results9.size(), outputShape.data(), outputShape.size());
//	//auto outputTensor10 = Ort::Value::CreateTensor<float>(memory_info, results10.data(), results10.size(), outputShape.data(), outputShape.size());
//
//	////入力画像取得（画像版）
//	//const std::string imageFile = "movieframe_1114.png";
//	//const cv::Mat imageFile = "movieframe_1114.png";
//	cout << "Get_image_flag =" << get_image_flag << endl;
//
//	cv::Mat imageMat = small_img;
//	//imwrite("movieframe_1114_2.png", small_img);
//	imshow("small_img2", small_img);
//	imwrite("small_img.png", small_img);
//	imwrite("movieframe.png", imageMat);
//	waitKey(1);
//	//cv::Mat imageMat = cv::imread(imageFile, cv::IMREAD_COLOR);
//
//	const std::vector<float> imageVec = loadImage(imageMat); // CHW形式を1次元にした画像データ
//	assert(imageVec.size() == numInputElements);
//
//	//入力画像取得（動画版）
//	//const std::vector<float> imageVec = loadImage(t_frame); // CHW形式を1次元にした画像データ
//	//std::cout << "start6666666666666666666666666666666666666666666666666666666666666666" << std::endl;
//	//assert(imageVec.size() == numInputElements); //サイズがあっているか確認
//
//	// 画像データを入力用配列にコピー
//	std::copy(imageVec.begin(), imageVec.end(), input.begin()); //imageVecの全部をinputに頭からコピー
//
//
//	// 入出力の名前
//// ONNXモデルの保存時と同じ名前にすること
//	const std::array<const char*, 1> inputNames = { "input0" };
//	const std::array<const char*, 1> outputNames = { "output0" };
//
//	std::vector<int> vector1;
//
//	//推論実行
//	Ort::RunOptions runOptions;
//	session.Run(runOptions, inputNames.data(), &inputTensor, 1, outputNames.data(), &outputTensor1, 1);
//
//	//推論結果の処理1
//	cout << "koko1" << std::endl;
//	torch::Tensor tsr1 = torch::tensor(torch::ArrayRef<float>(results1));
//	tsr1 = tsr1.reshape({ 103, width, height });
//	auto softmax1 = tsr1.softmax(0);
//	auto pred1 = softmax1.argmax(0);
//	auto one_hot1 = torch::nn::functional::one_hot(pred1);
//	one_hot1 = one_hot1.permute({ 2, 1, 0 });
//
//	auto out_tensor1 = one_hot1[101].mul(255).clamp(0, 255).to(torch::kU8);
//	cv::Mat mat1(width, height, CV_8U);
//	std::memcpy((void*)mat1.data, out_tensor1.data_ptr(), sizeof(torch::kU8) * out_tensor1.numel());
//	cv::Mat newmat1 = mat1 * 255;
//	//mat1 = mat1.reshape(0, 1);
//	//mat1.copyTo(vector1);
//
//	//imshow("newmat1", newmat1);
//	imwrite("newmat1.png", newmat1);
//	waitKey(1);
//
//	//torch::Tensor result = torch::tensor(torch::ArrayRef<int>(vector1));
//	//result = result.reshape({ 3, width, height });
//
//	//auto mode = torch::mode(result, 0);
//	//auto mode_tensor = get<0>(mode).to(torch::kU8);
//	//cout << "mode_tensor" << mode_tensor << std::endl;
//	cv::Mat mode_mat(80, 80, CV_8UC1);
//	//std::memcpy((void*)mode_mat.data, mode_tensor.data_ptr(), sizeof(torch::kU8) * mode_tensor.numel());
//	//cout << "mode_mat" << mode_mat << std::endl;
//	//flip(mode_mat, mode_mat, 0);
//	//mode_mat = mode_mat * 255;
//	cv::Mat mode_mat2(80, 80, CV_8UC1);
//
//	//最大の領域のみ抽出
//	std::vector<std::vector<cv::Point> > contours;
//	std::vector<cv::Vec4i> hierarchy;
//	cv::findContours(mode_mat, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
//	auto maxcontour = std::max_element(contours.begin(), contours.end(), [](std::vector<cv::Point> x, std::vector<cv::Point> y) {
//		return cv::contourArea(x) < cv::contourArea(y);
//		});
//
//	cv::Mat polar_mat = cv::Mat::zeros(mode_mat2.size(), CV_8UC3);
//	std::vector<std::vector<cv::Point> > contours2;
//
//	if (contours.empty())
//		std::cout << "no polar" << std::endl;
//	else
//		contours2.push_back(*maxcontour);
//	std::cout << "max contours" << contours2 << std::endl;
//	cv::drawContours(polar_mat, contours2, -1, cv::Scalar(255, 255, 255), -1);
//
//	cout << "koko2" << std::endl;
//
//	cv::Mat polar_mat2;
//	cv::resize(polar_mat, polar_mat2, cv::Size(), 488.0 / polar_mat.cols, 488.0 / polar_mat.rows);
//	//imshow("polar_mat", polar_mat2);
//	//waitKey(1);
//
//	//cv::resize(mode_mat, mode_mat, cv::Size(), 488.0 / mode_mat.cols, 488.0 / mode_mat.rows);
//	//imshow("mat_image", mode_mat);
//	//waitKey(1);
//
//	//元画像に円を描画
//	Mat Labelmg;
//	Mat stats;
//	Mat centroids;
//	int nLab = connectedComponentsWithStats(mode_mat, Labelmg, stats, centroids);
//	int Max = 0;
//	int MaxID = 0;
//
//	for (int i = 1; i < nLab; ++i) {//全てのラベルを回す
//		int* param = stats.ptr<int>(i);
//		if (Max < param[ConnectedComponentsTypes::CC_STAT_AREA]) {//最大値より大きければ
//			Max = param[ConnectedComponentsTypes::CC_STAT_AREA];//最大値を更新
//			MaxID = i;
//			trimmed_area = Max;
//		}
//	}
//
//	double* mom = centroids.ptr<double>(MaxID);
//	int centerX3 = (centerX * trimmed_width / 644) - width / 2;
//	int centerY3 = (centerY * trimmed_width / 488) - height / 2;
//	if (centerX3 < 0) {
//		centerX3 = 0;
//	}
//	if (centerX3 > width) {
//		centerX3 = width;
//	}
//	if (centerY3 < 0) {
//		centerY3 = 0;
//	}
//	if (centerY3 > height) {
//		centerY3 = height;
//	}
//
//	/*cout << "centerX" << centerX3 << std::endl;
//	cout << "centerY" << centerY3 << std::endl;*/
//	moX = static_cast<int>((mom[0] + centerX3) * 648 / trimmed_width);
//	moY = 488 - static_cast<int>((mom[1] + centerY3) * 488 / trimmed_width);
//	//moY = static_cast<int>((height - mom[1] + centerY3) * 488 / trimmed_width);
//
//	//int roi_x = centerX3, roi_y = centerY3;  // 画像を貼り付ける左上の点
//	//int roi_x = centerX, roi_y = man2_cpos_c[1]-160;  // 画像を貼り付ける左上の点
//
//
//	cout << "mom0" << mom[0] << "," << "mom1" << mom[1] << std::endl;
//
//	float radius;
//
//
//	circle_area = trimmed_area * (648 * 488) / (width * height);
//	radius = static_cast<int>(sqrt(circle_area / M_PI));
//	//if (isnan(uncertainty)) {
//	//	radius = 20;
//	//}
//	//else if (uncertainty > 1.0) {
//	//	radius = 100;
//	//}
//	//else if (uncertainty < 0.01) {
//	//	radius = 20;
//	//}
//	//else {
//	//	radius = 100 * sqrt(uncertainty);
//	//	//radius = 200 *uncertainty;
//	//}
//	radius_2 = radius;
//
//	//マスク画像作成
//	//cv::Mat bg = cv::Mat::zeros(160, 160, CV_8UC1); // 背景画像
//	bg = cv::Mat::zeros((int)(IMG_HEIGHT * mag_pointed_frame), (int)(IMG_WIDTH * mag_pointed_frame), CV_8UC1); // 背景画像
//	//cv::Mat polar_mat3;
//	
//	//polar_mat.convertTo(polar_mat3, CV_8UC1);
//	//cvtColor(newmat1, newmat1, cv::COLOR_RGB2GRAY);
//	cv::Mat newmat2; // 前景画像
//	newmat1.convertTo(newmat2, CV_8UC1);
//
//	//imshow("newmat2", newmat1);
//	//imwrite("newmat2.png", newmat1);
//
//	//cv::Mat fg = newmat1; // 前景画像
//	////cv::Mat fg;
//	//newmat1.copyTo(fg);
//	cv::resize(newmat2, newmat2, cv::Size(), 4.0, 4.0);
//	cout << newmat2.size() << std::endl;
//	cout << bg.size() << std::endl;
//	//cout << "roi = " << roi_x << "," << roi_y << endl;
//
//	cv::Mat roi = bg(cv::Rect(centerX, centerY, newmat2.cols, newmat2.rows));
//
//	//// 前景画像の画素を背景画像にコピー
//	//cv::Mat roi = bg(cv::Rect(roi_x, roi_y, fg.cols, fg.rows));
//	//fg.copyTo(roi);
//	//cv::resize(bg, mask2, cv::Size(), 648.0 / bg.cols, 488.0 / bg.rows);
//
//	// 前景画像の画素を背景画像にコピー
//	/*cv::Mat roi = bg(cv::Rect(roi_x, roi_y, fg.cols, fg.rows));*/
//	newmat2.copyTo(roi);
//	//cv::resize(bg, mask2, cv::Size(), 648.0 / bg.cols, 488.0 / bg.rows);
//
//	cv::imshow("mask_image", bg);
//	imwrite("mask.png", bg);
//	cv::waitKey(1);
//
//	//auto result2 = (result / 255).to(torch::kF32);
//	//auto var = at::var(result2, 0);
//	////cout << var << std::endl;
//	//auto sum = at::sum(var);
//	//auto sum2 = sum.item<float>();
//	//cout << "sum" << sum2 << std::endl;
//	////cout << pred_image << std::endl;
//	//auto nonzero = at::count_nonzero(one_hot3[100]);
//	//nonzero2 = nonzero.item<float>();
//	//cout << "nonzero" << nonzero << std::endl;
//	//uncertainty = sum2 / nonzero2;
//	//cout << "uncertainty" << uncertainty << std::endl;
//
//	/*if (nonzero2 > 1) {
//	cv::circle(view2, cv::Point(moX, moY), radius, cv::Scalar(0, 255, 0), 2, cv::LINE_4);
//	}
//	else {
//		cout << "Polar is not detected." << std::endl;
//	}
//
//	imshow("circle_image2", view2);
//
//	waitKey(1);*/
//	/*output << view2;*/
//	//std::cout << "finish" << std::endl;
//	//end = std::chrono::system_clock::now();  // 計測終了時間
//	//double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//	//std::cout << "time" << elapsed << std::endl;//推論に要した時間(ms)
//
//	int key = cv::waitKey(1);
//	//if (key == 'q') {
//
//	//	cv::destroyWindow("circle_image");
//	//	video.release();
//	//	break; //whileループから抜ける
//	//}
//
//
//}

unsigned __stdcall mythread10(LPVOID lpx)
{
	int MaxID = 0;
	for (;;) {
		WaitForSingleObject(hEvent[10], INFINITE);

		//if (active_flag == 2) {
		//	Get_circle_size(C_radius, C_center_x, C_center_y, man2_cpos_ex);
		//	C_radius_ex = C_radius * radius_ratio;
		//	man24_cpos_c[0] = (IMG_WIDTH * 0.4) + (Kc * (man2_cpos_ex[0] - point_man_x_s));
		//	man24_cpos_c[1] = (IMG_HEIGHT * 0.4) + (Kc * (man2_cpos_ex[1] - point_man_y_s));
		//	cout << "man2 is center" << endl;
		//}
		//else if (active_flag == 4) {
		//	Get_circle_size(C_radius, C_center_x, C_center_y, man4_cpos_ex);
		//	C_radius_ex = C_radius * radius_ratio;
		//	man24_cpos_c[0] = (IMG_WIDTH * 0.4) + (Kc * (man4_cpos_ex[0] - point_man_x_s));
		//	man24_cpos_c[1] = (IMG_HEIGHT * 0.4) + (Kc * (man4_cpos_ex[1] - point_man_y_s));
		//	cout << "man4 is center" << endl;
		//}

		//cout << "man24_cpos_c, Kc, man2_cpos_ex, point_man_x_s =" << man24_cpos_c[0] << "," << Kc << "," << man2_cpos_ex[0] << "," << point_man_x_s << endl;

		if (inside_count > 1 && center_flag == 0) {
			sleep(50); //吸引による胚の変形を待つ
			if (active_flag == 2) {
				Get_circle_size(C_radius, C_center_x, C_center_y, man2_cpos_ex);
				C_radius_ex = C_radius * radius_ratio;
				man24_cpos_c[0] = (IMG_WIDTH * 0.4) + (Kc * (man2_cpos_ex[0] - point_man_x_s));
				man24_cpos_c[1] = (IMG_HEIGHT * 0.4) + (Kc * (man2_cpos_ex[1] - point_man_y_s));
				cout << "man2 is center" << endl;
			}
			else if (active_flag == 4) {
				Get_circle_size(C_radius, C_center_x, C_center_y, man4_cpos_ex);
				C_radius_ex = C_radius * radius_ratio;
				man24_cpos_c[0] = (IMG_WIDTH * 0.4) + (Kc * (man4_cpos_ex[0] - point_man_x_s));
				man24_cpos_c[1] = (IMG_HEIGHT * 0.4) + (Kc * (man4_cpos_ex[1] - point_man_y_s));
				cout << "man4 is center" << endl;
			}

			transparency();

			// 画像サイズの検証
			if (bg.rows <= 0 || bg.cols <= 0) {
				std::cerr << "Error: Invalid image dimensions" << std::endl;
				sleep(10);
			}
			else {

				//最大の領域のみ抽出
				Mat stats;
				Mat centroids;
				int nLab = connectedComponentsWithStats(bg, Labelmg_slit, stats, centroids);
				int Max = 0;

				cout << "nLab =" << nLab << endl;

				for (int i = 1; i < nLab; ++i) {//全てのラベルを回す
					int* param = stats.ptr<int>(i);
					if (Max < param[ConnectedComponentsTypes::CC_STAT_AREA]) {//最大値より大きければ
						Max = param[ConnectedComponentsTypes::CC_STAT_AREA];//最大値を更新
						MaxID = i;
						trimmed_area = Max;

					}
				}

				int* param = stats.ptr<int>(MaxID);
				int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
				int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
				int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
				int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
				C_center_xc = x + width / 2;
				C_center_yc = y + height / 2;
				C_radius_c = (width + height) / 4.0;
				cout << "Ccenter =" << C_center_xc << "," << C_center_yc << endl;
				cout << "C_radius=" << C_radius_c << endl;
				cout << "Max_ID =" << MaxID << endl;
				sleep(100);
				inside_flag = 3;
				inside_count--;
			}

		}

		if (end_flag == 1) break;

	}
	return 0;
}

//data
unsigned __stdcall mythread11(LPVOID lpx)
{
	//std::vector<std::string> buffer; // ローカルバッファ
	std::vector<std::tuple<double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, int, double, double, double>> buffer; // 座標データのバッファ

	int loopCount = 0;
	const int maxLoops = 5; // 総ループ回数

	while (true) {
		WaitForSingleObject(hEvent[11], INFINITE);

		//データ記録用のファイルを作成（スペース＆Oボタン）
		if (file_flag == 0 && GetAsyncKeyState(VK_SPACE) && GetAsyncKeyState(0x4F)) {
			sprintf(fname, "data.csv");
			data_file1.open(fname, ios::app);
			if (!data_file1) cout << "出力ファイル1オープンに失敗\n" << endl;
			data_file1 << "time,man1_xw,man1_yw,man1_zw,man2_xw,man2_yw,man2_zw,man3_xw,man3_yw,man3_zw,man4_xw,man4_yw,man4_zw,man1_cpos[0],man1_cpos[1],man1_cpos[2],man2_cpos[0],man2_cpos[1],man2_cpos[2],man3_cpos[0],man3_cpos[1],man3_cpos[2],man4_cpos[0],man4_cpos[1],man4_cpos[2],target_angle[1],target_angle[3],phase_flag,Kc,slit_xc,slit_yc" << endl;

			sprintf(fname, "position.csv");
			data_file2.open(fname, ios::app);
			if (!data_file2) cout << "出力ファイル2オープンに失敗\n" << endl;
			data_file2 << "time,man1_xw,man1_yw,man1_zw,pPositionx,pPositiony,pPositionz,man1_2Dposx,man1_2Dposy,man1_2Dposz,pVelocityx,pVelocityry,pVelocityz,gain,m_flag,man1_cpos_exx,man1_cpos_exy,man1_mpoint_exx,man1_mpoint_exy";

			file_flag = true;
			sleep(1000);
			printf("csv_open");
		}

		//データ記録用のファイルを閉じる（スペース＆Cボタン）
		if (file_flag == true && GetAsyncKeyState(VK_SPACE) && GetAsyncKeyState(0x43)) {
			data_file1.close();
			data_file2.close();
			file_flag = false;
			printf("csv_close");
			file_number = file_number + 1;
			sleep(1000);
		}

		if (file_flag == true) {
			buffer.push_back(std::make_tuple(t, man1_xw, man1_yw, man1_zw, man2_xw, man2_yw, man2_zw, man3_xw, man3_yw, man3_zw, man4_xw, man4_yw, man4_zw, man1_cpos_ex[0], man1_cpos_ex[1], man1_cpos_ex[2], man2_cpos_ex[0], man2_cpos_ex[1], man2_cpos_ex[2], man3_cpos_ex[0], man3_cpos_ex[1], man3_cpos_ex[2], man4_cpos_ex[0], man4_cpos_ex[1], man4_cpos_ex[2], target_angle[1], target_angle[3], phase_flag, Kc, slit_xc, slit_yc));
			++loopCount;

			if (loopCount >= maxLoops) {
				// バッファ内容をCSVに書き込む
				//EnterCriticalSection(&cs);
				for (auto& data : buffer) {
					data_file1 << std::fixed << std::setprecision(2)
						<< std::get<0>(data) << "," << std::get<1>(data) << "," << std::get<2>(data) << "," << std::get<3>(data) << "," << std::get<4>(data) << "," << std::get<5>(data) << "," << std::get<6>(data) << "," << std::get<7>(data) << "," << std::get<8>(data) << "," << std::get<9>(data) << "," << std::get<10>(data)
						<< "," << std::get<11>(data) << "," << std::get<12>(data) << "," << std::get<13>(data) << "," << std::get<14>(data) << "," << std::get<15>(data) << "," << std::get<16>(data) << "," << std::get<17>(data) << "," << std::get<18>(data) << "," << std::get<19>(data) << "," << std::get<20>(data)
						<< "," << std::get<21>(data) << "," << std::get<22>(data) << "," << std::get<23>(data) << "," << std::get<24>(data) << "," << std::get<25>(data) << "," << std::get<26>(data) << "," << std::get<27>(data) << "," << std::get<28>(data) << "," << std::get<29>(data) << "," << std::get<30>(data) << "\n";
				}
				//LeaveCriticalSection(&cs);

				buffer.clear(); // ローカルバッファをクリア
				loopCount = 0;
			}
		}
		if (end_flag == 1) break;
		sleep(2);
	}

	data_file1.close();
	return 0;
}

//// 基本構造体の定義
//struct Vector3D {
//	double x;
//	double y;
//	double z;
//};

Vector3D center{ 2500, -2500, -2500 };
Vector3D set_pos1{ 2800,-1500,-2200 };
Vector3D set_pos2{ 2800,-3500,-2200 };
Vector3D out_pos1{ 4000,-1500,-2200 };
Vector3D out_pos2{ 4000,-3500,-2200 };
//std::vector<double> out_pos1{ set_pos13d[0] + 200,-set_pos13d[1],man2_auto_z };
//std::vector<double> out_pos2{ set_pos23d[0] + 200,-set_pos23d[1],man4_auto_z };
Vector3D wait_pos1{ 1450, -1650, -2200 };
Vector3D wait_pos2{ 1450, -3350, -2200 };
Vector3D cent_pos{ 3050,-2500, -2200 };
//std::vector<double> cent_pos2{ cent_pos23d[0]+400,-cent_pos23d[1],man4_auto_z +200};
Vector3D wait_pos1z{ 1450, -1450, -2200 };
Vector3D wait_pos2z{ 1450, -3550, -2200 };

int in_rev1 = 0;
int in_rev2 = 0;

// 制御システムの設定
struct ControlConfig {
	double position_tolerance;    // 位置制御の許容誤差
	int movement_timeout_ms;      // 移動のタイムアウト時間
	int rotate_timeout_ms;      // 移動のタイムアウト時間
	int hold_timeout_ms;         // 保持確認のタイムアウト時間
	int check_interval_ms;       // 位置チェックの間隔
	Vector3D base_speed;           // 基本速度
	double work_speed_factor;    // 作業時の速度係数
	double approach_speed_factor; // 接近時の速度係数

	ControlConfig()
		: position_tolerance(10.0)
		, movement_timeout_ms(1500)
		, rotate_timeout_ms(1500)
		, hold_timeout_ms(200)
		, check_interval_ms(5)
		, base_speed(1000, 1000, 1000)
		, work_speed_factor(0.5)
		, approach_speed_factor(0.7)
	{}
};

// メインコントローラークラス
class DualController {
private:
	bool is_initialized; //初期化フラグ

	// コントローラーの状態定義
	enum class ControllerState {
		IDLE,      // 待機状態
		MOVING,    // 移動中
		WORKING,   // 作業中
		ERROR_STATE      // エラー状態
	};

	enum class ActiveController {
		UPPER,    // 上部コントローラーがアクティブ
		LOWER     // 下部コントローラーがアクティブ
	};

	// コントローラー情報の構造体
	struct Controller {
		HANDLE& device;           // デバイス参照
		int angle_index;         // 角度制御のインデックス
		bool& hold_flag;         // 保持フラグの参照
		ControllerState state;   // 現在の状態
		Vector3D current_pos;    // 現在位置
		Vector3D target_pos;     // 目標位置
		bool is_upper;           // 上部コントローラーかどうか
		double in_rev;			 //吸引回転角
		double out_rev;			 //吐出回転角

		Controller(HANDLE& dev, int ang_idx, bool& flag, bool upper, double in_rev, double out_rev)
			: device(dev)
			, angle_index(ang_idx)
			, hold_flag(flag)
			, state(ControllerState::IDLE)
			, is_upper(upper)
			, in_rev(in_rev)
			, out_rev(out_rev)
		{}
	};

	// クラスのメンバ変数
	Controller upper_ctrl;
	Controller lower_ctrl;
	ControlConfig& config;
	bool is_exchanging;
	ActiveController current_active;

	// 位置制御関連の関数
	bool moveToPosition(Controller& ctrl, const Vector3D& target, Vector3D speed) {
		Movepos2(ctrl.device, speed, target);  // 実際のハードウェア制御コマンド

		int elapsed_time = 0;
		while (elapsed_time < config.movement_timeout_ms) {
			updatePosition(ctrl);

			if (isAtPosition(ctrl, target)) {
				sleep(100);
				return true;
			}

			sleep(config.check_interval_ms);
			elapsed_time += config.check_interval_ms;
		}

		return false;
	}

	void updatePosition(Controller& ctrl) {
		if (ctrl.is_upper) {
			Getposition(ctrl.device, man2_xw, man2_yw, man2_zw);
			ctrl.current_pos = { man2_xw, man2_yw, man2_zw };
			//std::cout << "man2_posw = " << man2_xw << "," << man2_yw << "," << man2_zw << endl;
		}
		else {
			Getposition(ctrl.device, man4_xw, man4_yw, man4_zw);
			ctrl.current_pos = { man4_xw, man4_yw, man4_zw };
			//std::cout << "man4_posw = " << man4_xw << "," << man4_yw << "," << man4_zw << endl;
		}
	}

	bool isAtPosition(const Controller& ctrl, const Vector3D& target) {
		return (std::abs(ctrl.current_pos.x - target.x) < config.position_tolerance &&
			std::abs(ctrl.current_pos.y - target.y) < config.position_tolerance &&
			std::abs(ctrl.current_pos.z - target.z) < config.position_tolerance);
	}

	// 経路計画関連の関数
	void planPath(Controller& ctrl, const Vector3D& start, const Vector3D& end, bool avoid_center) {
		std::vector<Vector3D> path;

		if (avoid_center) {
			// 中央を避ける経路の生成
			double z_offset = ctrl.is_upper ? 300.0 : -300.0;

			// 1. 現在位置から上/下に移動
			Vector3D point1 = start;
			point1.z += z_offset;
			path.push_back(point1);

			// 2. 目標位置の上/下に移動
			Vector3D point2 = end;
			point2.z += z_offset;
			path.push_back(point2);
		}

		// 最終目標位置を追加
		path.push_back(end);

		// パスに従って移動
		for (const auto& point : path) {
			if (!moveToPosition(ctrl, point, config.base_speed)) {
				//handleError(ctrl, "Path movement failed");
				return;
			}
		}
	}

	// 交換動作の実行
	void executeExchange() {
		Controller& active = (current_active == ActiveController::UPPER) ? upper_ctrl : lower_ctrl;
		Controller& waiting = (current_active == ActiveController::UPPER) ? lower_ctrl : upper_ctrl;

		if (active.is_upper) {
			pre_man24_cpos_ex[0] = man2_cpos_ex[0];
			pre_man24_cpos_ex[1] = man2_cpos_ex[1];
			pre_man24_pos[0] = man2_xw;
			pre_man24_pos[1] = man2_yw;
		}
		else {
			pre_man24_cpos_ex[0] = man4_cpos_ex[0];
			pre_man24_cpos_ex[1] = man4_cpos_ex[1];
			pre_man24_pos[0] = man4_xw;
			pre_man24_pos[1] = man4_yw;
		}

		// アクティブコントローラーの移動シーケンス
		// 1. cent_pos → set_pos (衝突回避用の中間点)
		moveToPosition(active,
			active.is_upper ? set_pos1 : set_pos2,
			config.base_speed);

		// 2. set_pos → out_pos
		moveToPosition(active,
			active.is_upper ? out_pos1 : out_pos2,
			config.base_speed);

		target_angle[active.angle_index] = active.out_rev;
		sleep(100);

		Move_Pipette_Getposition(man13_store); //操作しているピペットの位置を取得
		//Vector3D m_pos0{ man13_store[0] - 200, man13_store[1], cent_pos.z };
		Vector3D m_pos0{ 0,0,0 };

		if (center_flag == 1) {
			m_pos0 = { man13_store[0] - 200, man13_store[1], cent_pos.z };
		}
		else if (center_flag == 0) {
			m_pos0 = { pre_man24_pos[0], pre_man24_pos[1], cent_pos.z };
		}

		cout << "m_pos0 =" << m_pos0.x << "," << m_pos0.y << endl;
		active_flag = active.is_upper ? 4 : 2;

		// 作業位置のセットアップ
		setupWorkPosition(waiting);

		left_circle_flag = 1;

		// 3. 待機コントローラーをcent_posへ移動開始
		moveToPosition(waiting,
			m_pos0,
			config.base_speed);

		//target_angle[active.angle_index] = 0.2;

		//active_flag = active.is_upper ? 4 : 2;e
		//planPath(waiting,
		//	waiting.is_upper ? wait_pos1 : wait_pos2,
		//	cent_pos,uuuu
		//	true);

		// 4. アクティブコントローラーをwait_posへ
		moveToPosition(active,
			active.is_upper ? wait_pos1 : wait_pos2,
			config.base_speed);
		//planPath(active,
		//	active.is_upper ? out_pos1 : out_pos2,
		//	active.is_upper ? wait_pos1 : wait_pos2,
		//	true);

		// 2. 待機コントローラーの動作シーケンス開始
		if (NOC_auto > 0 || p_flag == 1) {
			// ターゲット位置の検出
			//left_circle_flag = 1;
			sleep(50);

			// コントローラーに応じた位置検出関数の呼び出し
			Vector3D target_pos;
			//if (waiting.is_upper) {
			if (active.is_upper) {
				circle_to_world1(t_3Dpos_ex);
				target_pos = {
					t_3Dpos_ex[0] - 100,
					-t_3Dpos_ex[1],
					man2_auto_z1
				};
			}
			else {
				circle_to_world2(t_3Dpos_ex);
				target_pos = {
					t_3Dpos_ex[0] - 100,
					-t_3Dpos_ex[1],
					man4_auto_z1
				};
			}

			// 吸引動作
			Rotatewithcheck(active.angle_index, active.in_rev);
			sleep(1000);

			// ターゲット位置への移動
			moveToPosition(active, target_pos, config.base_speed);
			cout << "target_pos =" << target_pos.x << endl;

			// 最終アプローチ位置への移動
			Vector3D final_pos = target_pos;
			final_pos.z = active.is_upper ? man2_auto_z2 : man4_auto_z2;
			moveToPosition(active, final_pos, config.base_speed);
			cout << "final_pos =" << final_pos.x << endl;

			sleep(2000);

			// 保持確認
			//if (active.is_upper) {
			//	check_inj1(active.hold_flag);
			//}
			//else {
			//	check_inj2(active.hold_flag);
			//}

			// 待機位置Zへ移動 20250308
			//moveToPosition(active,
				//active.is_upper ? wait_pos1z : wait_pos2z,
				//config.base_speed);

			// 保持状態の確認と処理
			//sleep(30);
			//if (!active.hold_flag) {
			//	target_angle[active.angle_index] = 0;
			//}
		}

		// 状態の更新
		active.state = ControllerState::IDLE;
		waiting.state = ControllerState::WORKING;
		current_active = waiting.is_upper ? ActiveController::UPPER : ActiveController::LOWER;

		//// 作業位置のセットアップ
		//setupWorkPosition(waiting);
	}

	// 作業位置のセットアップ
	void setupWorkPosition(Controller& ctrl) {
		height_flag = 1;
		inside_count = 0;
		inside_flag = 0;
		//left_circle_flag = 1;
		pipette_set_b();

		// フォーカス調整
		//if (center_flag == 0) {
		//	zoom_cancel();
		//}

		sleep(100);
		if (center_flag == 1) zoom(man1_cpos_ex[0] - 40, man1_cpos_ex[1]);
		sleep(100);
	}

	// 作業位置のセットアップ
	void setupWorkPosition0(Controller& ctrl) {
		height_flag = 1;
		inside_count = 0;
		inside_flag = 0;
		//left_circle_flag = 1;
		pipette_set_a();

		// フォーカス調整
		if (center_flag == 0) {
			zoom_cancel();
		}
		sleep(100);
		zoom(man1_cpos_ex[0] - 40, man1_cpos_ex[1]);
	}

	// エラー処理
	void handleError(Controller& ctrl, const std::string& message) {
		std::cerr << "Error in " << (ctrl.is_upper ? "upper" : "lower")
			<< " controller: " << message << std::endl;

		ctrl.state = ControllerState::ERROR_STATE;

		// 安全な位置への移動
		moveToPosition(ctrl,
			ctrl.is_upper ? wait_pos1 : wait_pos2,
			config.base_speed);

		// 角度のリセット
		target_angle[ctrl.angle_index] = 0;
	}

public:
	DualController(HANDLE& upper_dev, HANDLE& lower_dev,
		int upper_ang_idx, int lower_ang_idx,
		bool& upper_flag, bool& lower_flag,
		ControlConfig& cfg)
		: upper_ctrl(upper_dev, upper_ang_idx, upper_flag, true, 0.8, 0.0)
		, lower_ctrl(lower_dev, lower_ang_idx, lower_flag, false, 0.8, 0.0)
		, config(cfg)
		, is_exchanging(false)
		, current_active(ActiveController::UPPER)
		, is_initialized(false)
	{}

	// システムの初期化
	void initialize() {
		try {
			// フラグの初期化
			height_flag = 1;
			inside_count = 0;
			inside_flag = 0;
			//left_circle_flag = 1;
			auto_step = 1;
			auto_flag = 1;

			// 初期位置への移動
			moveToPosition(upper_ctrl, cent_pos, config.base_speed);
			moveToPosition(lower_ctrl, wait_pos2, config.base_speed);

			// 初期状態の設定
			upper_ctrl.state = ControllerState::WORKING;
			lower_ctrl.state = ControllerState::IDLE;
			current_active = ActiveController::UPPER;

			setupWorkPosition0(upper_ctrl);
		}
		catch (const std::exception& e) {
			std::cerr << "Initialization error: " << e.what() << std::endl;
			throw;
		}
	}

	// メインの更新関数
	void update() {
		try {
			if (GetAsyncKeyState(0x33) && GetAsyncKeyState(0x5A)) {
				if (!is_initialized) {
					initialize();
					std::cout << "わんこそばinitialize\n" << std::endl;
					is_initialized = true;
					return;
				}
				else if (is_initialized) {
					auto_flag = 0;
					std::cout << "わんこそばstop\n" << endl;
					phase_flag = 0;
					is_initialized = false;
					sleep(1000);
					return;
				}
			}
			//else if (GetAsyncKeyState(0x33)) { //Z+3ボタン;半自動搬送モードの切り替え //GetAsyncKeyState(0x5A)
//	if (auto_flag == 0) {
//		auto_flag = 1;
//		auto_step = 1;
//		Getposition(hDevice_LD, man4_xw, man4_yw, man4_zw);
//		Getposition(hDevice_LU, man2_xw, man2_yw, man2_zw);
//		std::cout << "半自動" << auto_flag << std::endl;
//	}
//	else if (auto_flag == 1) {
//		auto_flag = 0;
//		std::cout << "手動" << auto_flag << std::endl;
//	}
//}

			// キー入力のチェック
			if (GetAsyncKeyState(0x49) && !is_exchanging && is_initialized) {
				is_exchanging = true;
				executeExchange();
				is_exchanging = false;
			}

			if (GetAsyncKeyState(0x45)) {
				cout << "shutdown requested" << endl;
				shutdown();
			}

			//cout << "is_exchanging =" << is_exchanging << endl;

			// 各コントローラーの状態更新
			updatePosition(upper_ctrl);
			updatePosition(lower_ctrl);
		}
		catch (const std::exception& e) {
			std::cerr << "Update error: " << e.what() << std::endl;
			//handleError(upper_ctrl, "Update failed");
			//handleError(lower_ctrl, "Update failed");
		}
	}

	// システムのシャットダウン
	void shutdown() {
		try {
			// 両方のコントローラーを安全な位置に移動
			moveToPosition(upper_ctrl, wait_pos1, config.base_speed);
			moveToPosition(lower_ctrl, wait_pos2, config.base_speed);

			// 角度のリセット
			target_angle[upper_ctrl.angle_index] = 0;
			target_angle[lower_ctrl.angle_index] = 0;

			active_flag == 0;
		}
		catch (const std::exception& e) {
			std::cerr << "Shutdown error: " << e.what() << std::endl;
		}
	}

	// 回転制御関数
	bool Rotatewithcheck(int ChNo, double target) {
		target_angle[ChNo] = target;
		int elapsed_time = 0;
		while (elapsed_time < config.rotate_timeout_ms) {
			if (abs(get_revolution(ChNo)) < 0.1) return true;
			sleep(config.check_interval_ms);
			elapsed_time += config.check_interval_ms;
		}

		return false;
	}
};

//wankosoba
unsigned __stdcall mythread12(LPVOID lpx)
{
	try {
		// 設定の初期化
		ControlConfig config;
		config.position_tolerance = 10.0;
		config.movement_timeout_ms = 1500;
		config.rotate_timeout_ms = 1500;
		config.hold_timeout_ms = 200;
		config.check_interval_ms = 5;
		config.base_speed = { 1000,1000,1000 };  // グローバル変数sを使用

		// フラグの初期化
		bool hold_flag2 = false;
		bool hold_flag4 = false;

		// コントローラーシステムの作成
		DualController controller(
			hDevice_LU, hDevice_LD,
			3, 1,  // angle_index for upper and lower
			hold_flag2, hold_flag4,
			config
		);

		// システムの初期化
		//controller.initialize();

		for (;;) {
			WaitForSingleObject(hEvent[12], INFINITE);

			// メインループ
			bool running = true;
			while (running) {
				controller.update();
				sleep(10);  // 適切な間隔での更新
			}
			// シャットダウン
			controller.shutdown();
			return 0;

			if (end_flag == 1) break;
		}
		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return 1;
	}
}


void multithread_controll() {
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	initialize_parameters();
	for (int i = 0; i < 11; i++) {
		hTh[i] = nullptr;
		hEvent[i] = nullptr;
	}

	ofstream mirror_log("logging\\mirror.csv");
	ofstream mirror_log2("logging\\mirror2.csv");

	mirror_log << "count" << "," << "pimage" << endl;
	mirror_log2 << "count" << "," << "pangle" << "," << "xangle" << "," << "yangle" << endl;
	int pre_Ximage_creation = 1;	//拡張画像合成処理の開始タイミングの前倒しを行うための変数

	//double s_dWD2 = s_dWD;
	double MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;
	double t_MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;
	frame_width = (2 * (l2 + l3) * 1000 * dtan(*plist[X - 1]) + 2 * IMG_WIDTH) / resolution;
	frame_height = (2 * (l3) * 1000 * dtan(*(plist[X - 1] + ndata[X - 1] / 4 * 4 + 1)) + 2 * IMG_HEIGHT) / resolution;
	printf("frame_width=%d,frame_height%d\n", frame_width, frame_height);

	countX = 0;
	count_ndata = 0;
	count_Xndata = 0;
	pointed_count = 0;

	LARGE_INTEGER qpc3;
	LARGE_INTEGER qpc4;
	LARGE_INTEGER s_pointed_waittime;
	LARGE_INTEGER t_pointed_waittime;
	double pointed_waittime;
	double Time2[nstoreimage];
	double Time3[nstorepointedimage];
	ofstream multi_thrd_ctrl_looptime("logging\\looptime.csv");
	LARGE_INTEGER loop_st;
	LARGE_INTEGER loop_end;
	LARGE_INTEGER loop_st_X;
	LARGE_INTEGER loop_end_X;
	double loop_time = 0;
	double loop_time_X = 0;
	multi_thrd_ctrl_looptime << "count" << "," << "time" << endl;

	isClick = true;


	double s_WD = l1 + l2 + l3;

	ClTest.init_settings(MAG_ExpandedIMG, s_MAG_ExpandedIMG, frame_height, frame_width);

	unsigned int thID[13];
	DWORD dwExCode[13];
	int thread_maxNo = 13;
	//マルチスレッドする際の魔法の言葉
	hTh[0] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread0,
		NULL,
		CREATE_SUSPENDED,
		&thID[0]
	);
	if (hTh[0] == NULL) {
		printf("スレッド0作成失敗\n");
		exit(0);
	}

	hTh[1] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread1,
		NULL,
		CREATE_SUSPENDED,
		&thID[1]
	);
	if (hTh[1] == NULL) {
		printf("スレッド1作成失敗\n");
		exit(0);
	}

	hTh[2] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread2,
		NULL,
		CREATE_SUSPENDED,
		&thID[2]
	);
	if (hTh[2] == NULL) {
		printf("スレッド2作成失敗\n");
		exit(0);
	}


	hTh[3] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread3,
		NULL,
		CREATE_SUSPENDED,
		&thID[3]
	);
	if (hTh[3] == NULL) {
		printf("スレッド3作成失敗\n");
		exit(0);
	}

	hTh[4] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread4,
		NULL,
		CREATE_SUSPENDED,
		&thID[4]
	);
	if (hTh[4] == NULL) {
		printf("スレッド4作成失敗\n");
		exit(0);
	}

	hTh[5] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread5,
		NULL,
		CREATE_SUSPENDED,
		&thID[5]
	);
	if (hTh[5] == NULL) {
		printf("スレッド5作成失敗\n");
		exit(0);
	}
	hTh[6] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread6,
		NULL,
		CREATE_SUSPENDED,
		&thID[6]
	);
	if (hTh[6] == NULL) {
		printf("スレッド6作成失敗\n");
		exit(0);
	}
	hTh[7] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread7,
		NULL,
		CREATE_SUSPENDED,
		&thID[7]
	);
	if (hTh[7] == NULL) {
		printf("スレッド7作成失敗\n");
		exit(0);
	}
	hTh[8] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread8,
		NULL,
		CREATE_SUSPENDED,
		&thID[8]
	);
	if (hTh[8] == NULL) {
		printf("スレッド8作成失敗\n");
		exit(0);
	}
	hTh[9] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread9,
		NULL,
		CREATE_SUSPENDED,
		&thID[9]
	);
	if (hTh[9] == NULL) {
		printf("スレッド9作成失敗\n");
		exit(0);
	}
	hTh[10] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread10,
		NULL,
		CREATE_SUSPENDED,
		&thID[10]
	);
	if (hTh[10] == NULL) {
		printf("スレッド10作成失敗\n");
		exit(0);
	}
	hTh[11] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread11,
		NULL,
		CREATE_SUSPENDED,
		&thID[11]
	);
	if (hTh[11] == NULL) {
		printf("スレッド11作成失敗\n");
		exit(0);
	}
	hTh[12] = (HANDLE)_beginthreadex(
		NULL,
		0,
		mythread12,
		NULL,
		CREATE_SUSPENDED,
		&thID[12]
	);
	if (hTh[12] == NULL) {
		printf("スレッド12作成失敗\n");
		exit(0);
	}

	//hEvent[0] = CreateEvent(NULL, TRUE, FALSE, "ENDEVENT");
	hEvent[0] = CreateEvent(NULL, TRUE, FALSE, "input_ETL_Temp");
	hEvent[1] = CreateEvent(NULL, TRUE, FALSE, "get&create_img");
	hEvent[2] = CreateEvent(NULL, TRUE, FALSE, "show_img");
	hEvent[3] = CreateEvent(NULL, TRUE, FALSE, "update_imagebuff");
	//hEvent[3] = CreateEvent(NULL, TRUE, FALSE, "input_keybord");
	hEvent[4] = CreateEvent(NULL, TRUE, FALSE, "input_Transferman");
	hEvent[5] = CreateEvent(NULL, TRUE, FALSE, "input_Transferman");
	hEvent[6] = CreateEvent(NULL, TRUE, FALSE, "input_Transferman");
	hEvent[7] = CreateEvent(NULL, TRUE, FALSE, "xbox_controll");
	hEvent[8] = CreateEvent(NULL, TRUE, FALSE, "phantom_controll");
	hEvent[9] = CreateEvent(NULL, TRUE, FALSE, "phantom_out");
	hEvent[10] = CreateEvent(NULL, TRUE, FALSE, "inference");
	hEvent[11] = CreateEvent(NULL, TRUE, FALSE, "csv");
	hEvent[12] = CreateEvent(NULL, TRUE, FALSE, "wankosoba");

	//各スレッド実行開始
	for (int thread_No = 0; thread_No < thread_maxNo; thread_No++)
		ResumeThread(hTh[thread_No]);

	printf("イベント0~%dをシグナル状態にします\n", thread_maxNo - 1);

	//イベントオブジェクトをシグナル状態へ
	for (int thread_No = 0; thread_No < thread_maxNo; thread_No++) {
		if (hEvent[thread_No] != nullptr) {
			SetEvent(hEvent[thread_No]);
			//cout << thread_No << endl;
		}
	}
	sleep(100);


	//double s_xangle=s_mirror_x*2;
	//double s_yangle=s_mirror_y*2;

	/*ETLの初期設定*/
	//xangle = 0, yangle = 0のときのWD, dWDを算出。fObETOLとfTLの値を保存し、キャリブレーションモードに使用。
	WD = derive_WD(xangle, yangle);
	dWD = WD - s_WD + s_dWD2;
	send_temperaturereading_command();
	calculate_etl2(dWD);
	//pData[0] = 32767 / 10 * s_mirror_x + 32767;
	//pData[1] = 32767 / 10 * s_mirror_y + 32767;
	pData[0] = 32767 / 20 * s_mirror_x + 32767;
	pData[1] = 32767 / 20 * s_mirror_y + 32767;

	//DAの出力
	DaOutputDA(ghDeviceHandle_da1, 2, &gstrDaSmplChReq[0], &pData[0]);
	if (current > max_current) current = max_current;
	else if (current < 0) current = 0;

	send_currentset_command(current);
	sleep(10);

	int Xtcount = 0;
	expand_viewpoint_num = make_angle_table2();
	double* p = plist_angle[0];
	printf("expand_viewpoint_num=%d\n", expand_viewpoint_num);
	int store_image_count_tmp = 0;
	store_image_count = 0;
	store_angle_count = 0;
	QueryPerformanceCounter(&qpc3);
	QueryPerformanceCounter(&qpc0);
	QueryPerformanceCounter(&s_pointed_waittime);
	int X_th_num = 0;

	// ★ AIのROIを保持する変数、実効fpsをループ外で宣言（大島追加）
	cv::Rect ai_roi;
	double display_fps = 0.0;

	for (;;) {
		QueryPerformanceCounter(&start);
		//データ記録用のファイルを作成（スペース＆Oボタン）
		//if (file_flag == 0 && GetAsyncKeyState(VK_SPACE) && GetAsyncKeyState(0x4F)) {
		//	sprintf(fname, "time.csv");
		//	data_file1.open(fname, ios::app);
		//	if (!data_file1) cout << "出力ファイル1オープンに失敗\n" << endl;
		//	data_file1 << "time,DAData1,count1,dwcount1,DAData2,count2,dwcount2,DAData3,count3,dwcount3,DAData4,count4,dwcount4" << endl;

		//	sprintf(fname, "position.csv");
		//	data_file2.open(fname, ios::app);
		//	if (!data_file2) cout << "出力ファイル2オープンに失敗\n" << endl;
		//	data_file2 << "time,man1_xw,man1_yw,man1_zw,pPositionx,pPositiony,pPositionz,man1_2Dposx,man1_2Dposy,man1_2Dposz,pVelocityx,pVelocityry,pVelocityz,gain,m_flag,man1_cpos_exx,man1_cpos_exy,man1_mpoint_exx,man1_mpoint_exy";

		//	file_flag = true;
		//	sleep(1000);
		//	printf("time");
		//}

		////データ記録用のファイルを閉じる（スペース＆Cボタン）
		//if (file_flag == true && GetAsyncKeyState(VK_SPACE) && GetAsyncKeyState(0x43)) {
		//	data_file1.close();
		//	data_file2.close();
		//	file_flag = false;
		//	printf("time_close");
		//	file_number = file_number + 1;
		//	sleep(1000);
		//}

		if (switch_flag == 1)
		{
			//大島追加
			// 拡張視野に切り替わったら、をリセット
			if (g_ai_motor_engaged.load() || g_ai_capture_request.load()) {
				g_ai_motor_engaged.store(false);
				g_ai_capture_request.store(false);
				g_ai_progress.store(0.0);
				g_ai_is_stop.store(false); // ★ 念のため
				g_ai_hidden_h.reset();     // ★ 内部状態をリセット
				g_ai_hidden_c.reset();     // ★ 内部状態をリセット
				g_locked_roi_width.store(0);  // ★ ロックしたROIをリセット
				g_locked_roi_height.store(0); // ★ ロックしたROIをリセット
				g_ai_initial_frame_mat.release(); // ★ 前のフレームもリセット
				g_ai_stop_counter.store(0); // ★ 連続STOPカウンターもリセット
				std::cout << "[AI] Resetting (Switched to Expanded View)" << std::endl;
			}
			//追加ここまで

			s_dWD = s_dWD2;
			//if (thread_start_num < 20) thread_start_num++;

			//if (t_MAG != MAG) printf("countndata=%d,countX=%d\n",count_ndata,countX);
			QueryPerformanceCounter(&loop_st);

			if (t_REC_switch == !REC_switch) {
				printf("Recording_start");
				store_image_count = 0;
				store_pointed_image_count = 0;
				count_ndata = 0;
				countX = 0;
				npointed_count2 = 0;
				QueryPerformanceCounter(&qpc3);
				p = plist_angle[countX];
				end_flag = SetEvent(hEvent[0]);
			}

			//範囲制限のある拡張画像から，現在の画像番号(count_Xndata)の角度情報(*p,*(p + 1))を取得
			p = plist_angle[count_Xndata];
			xangle = *p;
			yangle = *(p + 1);
			//printf("countX=%d,pointer=%p,xangle=%f,yangle=%f\n", countX, p, xangle, yangle);

			//各楕円の0番目の画像の時だけ実行（焦点距離の変更）
			if (count_ndata == 0) {
				WD = derive_WD(xangle, yangle);
				//WD=derive_WD(0.0, 0.);
				dWD = WD - s_WD + s_dWD2;
				calculate_etl2(dWD);
				if (current > max_current) current = max_current;
				else if (current < 0) current = 0;
				send_currentset_command(current);
				//cout << "current =" << current << endl;
				//sleep(1);
				//if (countX == 0) sleep(1);
				//printf("倍率=%f\n",magnification);
			}

			//x,yミラー角をdegに変換

			mirror_x = xangle / 2 + s_mirror_x;
			mirror_y = yangle / 2 + s_mirror_y;


			//x,yミラー角が範囲外になるときにメッセージを出力
			warn_mirror_range(mirror_x, mirror_y);

			int _a = 32767;

			//printf("xangle=%f", xangle);
			//pData[0] = 32767 / 10 * mirror_x + 32767;
			//pData[1] = 32767 / 10 * mirror_y + 32767;
			pData[0] = _a / 20 * mirror_x + _a;
			pData[1] = _a / 20 * mirror_y + _a;

			//DAの出力
			nRet = DaOutputDA(ghDeviceHandle_da1, 2, &gstrDaSmplChReq[0], &pData[0]);

			//if (countX == 0) sleep(1);

			//sleep(0.2);
			sleep(0.2);  //20241107 変更決定

			//----- Ad_input -----
			nRet = AdInputAD(hDeviceHandle_ad, 2, AD_INPUT_SINGLE, AdSmplChReq, data_ad);
			if (nRet != AD_ERROR_SUCCESS) {
				printf("AdInputAD11 errr1(%lx)", nRet);
				AdClose(hDeviceHandle_ad);
				printf("error");
			}
			//switch (countX)
			//{
			//case 0:
			//	sleep(2);
			//	break;
			//case 1:
			//	if (count_Xndata == 0) {
			//		sleep(2);
			//	}
			//	else {
			//		sleep(4);
			//	}
			//	break;
			//case 2:
			//	if (count_Xndata == 0) {
			//		sleep(2);
			//	}
			//	else {
			//		sleep(1);
			//	}
			//	break;

			//}
			//xi_stat = xiGetImage(xiH, 0, &image);
			//t_imagenframe = image.nframe;
			//sleep(2.4);
			sleep(2.2);
			cap >> img_phot;
			//cap >> circle_img;
			//flip(circle_img, circle_img, 0);
			//namedWindow("circle_img");
			//sleep(1.0);
			//cap >> img_phot;
			//sleep(2.0);

			//img_phot = Mat(img_phot, Rect(0, 0, IMG_WIDTH, IMG_HEIGHT));
			flip(img_phot, image_phot, 0);
			//std::string error_message2;
			//if (!validateImage2(image_phot, error_message)) {
			//	std::cerr << "Image validation failed: " << error_message << std::endl;
			//	return;
			//}

			//flip(image_phot, image_phot, 0);
			//imshow("frame", image_phot);
			//waitKey(1);
			//printf("mirrorx_diff=%f\n", (double)(data_ad[0] - 32767) / 32767 * 20 - (xangle / 2 + s_mirror_x));
			//printf("mirrory_diff=%f\n", (double)(data_ad[1] - 32767) / 32767 * 20 - (yangle / 2 + s_mirror_y));
			//cout << pData[0] << "," << data_ad[0] << "," << pData[1] << "," << data_ad[1] << endl;
			//printf("time=%f\n", time);
			//カメラからイメージ取得

			//while (t_imagenframe == image.nframe) {
			//	xi_stat = xiGetImage(xiH, 0, &image);
			//	QueryPerformanceCounter(&qpc4);
			//}
			sleep(2.82); //これ増やすと、選択したときに安定する 2.8~2.9で安定する
			//Mat view(IMG_HEIGHT2, IMG_WIDTH2,  CV_8UC1);
			//view.data = (uchar*)image.bp;


			mtx1.lock();

			if (delay_pointedframe <= 0) {
				//if (store_image_count >= 0) {
				if (store_image_count - delay_frame >= 0) {
					transport_image_multi_phot(image_phot, pimage[store_image_count - delay_frame]);
					//transport_image_multi_phot(image_phot, pimage[store_image_count]);
					//transport_image_multi(image, pimage[store_image_count - delay_frame]);
				}
				else {
					//transport_image_multi_phot(image_phot, pimage[nstoreimage + (store_image_count )]);
					transport_image_multi_phot(image_phot, pimage[nstoreimage + (store_image_count - delay_frame)]);

					//transport_image_multi(image, pimage[nstoreimage + (store_image_count - delay_frame)]);
				}
				store_image_count_tmp = store_image_count;
				store_image_count++;
				if (store_image_count >= nstoreimage) {
					count_nstoreimage++;
					store_image_count = 0;
				}
			}
			else {

				//if (store_pointed_image_count - 2 > 0) transport_image_multi(image, ppointedimage[nstoreimage * (pointed_count - delay_pointedframe) + store_pointed_image_count - 2]);
				if (store_pointed_image_count - 2 > 0) {
					transport_image_multi_phot(image_phot, ppointedimage[store_pointed_image_count - 2]);

					//transport_image_multi(image, ppointedimage[store_pointed_image_count - 2]);
				}
				//else transport_image_multi(image, ppointedimage[nstoreimage * (pointed_count - delay_pointedframe) + nstoreimage - 1]);
				delay_pointedframe--;
			}
			//xangle =(double)(data_ad[0]-32767)*10/32767*2*2;
			//yangle =(double)(data_ad[1]-32767)*10/32767*2*2;
			mirror_log2 << store_angle_count << "," << xangle << "," << yangle << endl;
			*pangle[store_angle_count] = xangle;
			*(pangle[store_angle_count] + 1) = yangle;
			QueryPerformanceCounter(&qpc4);
			Time2[store_image_count] = ((double)qpc4.QuadPart - (double)qpc3.QuadPart) / (double)qpf.QuadPart * 1000;
			//printf("TiME=%f,nframe=%d, pointer=%p\n",Time2[store_image_count],image.nframe, (uchar*)image.bp);

			ClTest.disable_pre_Xframe_creation(pre_Ximage_creation);
			//QueryPerformanceCounter(&loop_end);
			//loop_time = ((double)loop_end.QuadPart - (double)loop_st.QuadPart) / (double)qpf.QuadPart * 1000;
			//multi_thrd_ctrl_looptime << count_Xndata << "," << loop_time << endl;

			//cout << "koko" << endl;

			if (count_Xndata == expand_viewpoint_num - 1 - pre_Ximage_creation) {
				//cout << expand_viewpoint_num << "," << count_Xndata << endl;
				//cout << "Resume" << endl;
				//cout << store_image_count << "," << count_Xndata << endl;
				//cout << "count_Xndata, expand_viewpoint_num - 1 - pre_Ximage_creation =" << count_Xndata << "," << expand_viewpoint_num - 1 - pre_Ximage_creation << endl;
				QueryPerformanceCounter(&loop_end_X);
				loop_time_X = ((double)loop_end_X.QuadPart - (double)loop_st_X.QuadPart) / (double)qpf.QuadPart * 1000;
				QueryPerformanceCounter(&loop_st_X);
				//cout << "loop_st_X =" << loop_time_X << endl;

				//sleep(10);

				//拡張画像合成に用いる画像番号の保存
				ClTest.save_ExpCount_data(store_image_count_tmp + pre_Ximage_creation, count_Xndata + pre_Ximage_creation, expand_viewpoint_num, MAG);

				X_th_num++;
				//合成スレッドの稼働指令
				if (X_th_num == 1) {
					//合成スレッド１を稼働状態へ
					ResumeThread(hTh[1]);
					//X_th_num = 0;	//シングルスレッドで生成するとき
					//cout << "go_th1" << endl;

				}
				else if (X_th_num == 2) {
					//合成スレッド2を稼働状態へ
					ResumeThread(hTh[4]);
					//cout << "go_t4" << endl;
					//X_th_num = 0;
				}
				else if (X_th_num == 3) {
					//合成スレッド3を稼働状態へ
					ResumeThread(hTh[6]);
					//X_th_num = 0;
					//cout << "go_th6" << endl;
				}
				else if (X_th_num == 4) {	//現状，親スレッドが90ms，子スレッドが270~280msなため，最速で稼働させるには１休止必要．最速処理に重点を置くのであれば，親スレッドの処理時間，合成時間が変化した場合は変更する必要あり．
					X_th_num = 0;
				}
			}

			store_angle_count++;
			if (store_angle_count >= nstoreimage) store_angle_count = 0;
			//printf("count_Xndata=%d\n", count_Xndata);
			count_ndata++;
			count_Xndata++;

			if (count_ndata >= ndata2[countX]) {
				count_ndata = 0;
				countX++;
				if (countX >= X2) {
					t_MAG = MAG;
					MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;
					if (t_MAG != MAG) {
						expand_viewpoint_num = make_angle_table2();
						pre_Ximage_creation = expand_viewpoint_num - 1;

						printf("X2=%d, expand_viewpoint_num=%d,ndata=%d,%d,%d\n", X2, expand_viewpoint_num, ndata2[0], ndata2[1], ndata2[2]);
					}
					s_dWD2 = s_dWD;
					countX = 0;
					count_Xndata = 0;
					//ResumeThread(hTh[1]);
				}
			}
			mtx1.unlock();


			//選択された視点の撮像
			QueryPerformanceCounter(&t_pointed_waittime);
			pointed_waittime = ((double)t_pointed_waittime.QuadPart - (double)s_pointed_waittime.QuadPart) / (double)qpf.QuadPart * 1000;
			//printf("pointed_waittime=%f\n", pointed_waittime);
			if (pointed_waittime > 30 - (pointed_count - 1) * 4 && pointed_count > 0) {
				QueryPerformanceCounter(&s_pointed_waittime);
				//printf("pointed_waittime=%f\n",pointed_waittime);

				for (int npointed_count = 0; npointed_count < pointed_count; npointed_count++) {
					if (npointed_count == 0)delay_pointedframe = delay_frame;
					xangle = pointed_mirrorx[npointed_count] * 2;
					yangle = pointed_mirrory[npointed_count] * 2;
					WD = pointed_WD[npointed_count];
					dWD = WD - s_WD + s_dWD;
					calculate_etl2(dWD);
					if (current > max_current) current = max_current;
					else if (current < 0) current = 0;
					send_currentset_command(current); //中央の焦点変えられる

					//xi_stat = xiGetImage(xiH, 0, &image);
					//t_imagenframe = image.nframe;
					//cap >> img_phot;
					//flip(img_phot, image_phot, 0);
					sleep(5);
					////カメラからイメージ取得
					//while (t_imagenframe + 5 >= image.nframe) {
					//	xi_stat = xiGetImage(xiH, 0, &image);
					//}

					//sleep(1.6);

					mirror_x = pointed_mirrorx[npointed_count] + s_mirror_x;
					mirror_y = pointed_mirrory[npointed_count] + s_mirror_y;

					//x,yミラー角が範囲外になるときにメッセージを出力
					warn_mirror_range(mirror_x, mirror_y);

					//printf("xangle=%f", xangle);
					pData[0] = 32767 / 20 * mirror_x + 32767;
					pData[1] = 32767 / 20 * mirror_y + 32767;



					//DAの出力
					DaOutputDA(ghDeviceHandle_da1, 2, &gstrDaSmplChReq[0], &pData[0]);


					//----- Ad_input -----
					nRet = AdInputAD(hDeviceHandle_ad, 2, AD_INPUT_SINGLE, AdSmplChReq, data_ad);
					if (nRet != AD_ERROR_SUCCESS) {
						printf("AdInputAD11 errr1(%lx)", nRet);
						AdClose(hDeviceHandle_ad);
						printf("error");
					}

					//カメラからイメージ取得

					sleep(0.2);

					//xi_stat = xiGetImage(xiH, 0, &image);
					//t_imagenframe = image.nframe;
					//cap >> img_phot;
					//flip(img_phot, image_phot, 0);

					//printf("time=%f\n", time);
					//カメラからイメージ取得
					//while (t_imagenframe == image.nframe) {
					//	xi_stat = xiGetImage(xiH, 0, &image);
					//	QueryPerformanceCounter(&qpc4);
					//}
					sleep(2.0);  //202411107 変更決定
					cap >> img_phot;
					flip(img_phot, image_phot, 0);
					QueryPerformanceCounter(&qpc4);

					if (npointed_count - delay_frame < 0) {
						//if (store_image_count - delay_frame > 0)transport_image_multi(image, pimage[store_image_count - delay_frame]);
						//else transport_image_multi(image, pimage[nstoreimage - 1]);
						if (store_image_count - delay_frame >= 0) {
							transport_image_multi_phot(image_phot, pimage[store_image_count - delay_frame]);
						}
						else {
							transport_image_multi_phot(image_phot, pimage[nstoreimage + (store_image_count - delay_frame)]);
						}
						store_image_count++;
						if (store_image_count >= nstoreimage) {
							count_nstoreimage++;
							store_image_count = 0;
						}
					}
					else {
						transport_image_multi_phot(image_phot, ppointedimage[nstoreimage * (npointed_count - delay_frame) + store_pointed_image_count]);
					}

					//xangle =(double)(data_ad[0]-32767)*10/32767*2*2;
					//yangle =(double)(data_ad[1]-32767)*10/32767*2*2;
					*ppointedangle[nstoreimage * npointed_count + store_pointed_image_count] = xangle;
					*(ppointedangle[nstoreimage * npointed_count + store_pointed_image_count] + 1) = yangle;
					QueryPerformanceCounter(&qpc4);
					Time3[nstoreimage * npointed_count + store_pointed_image_count] = ((double)qpc4.QuadPart - (double)qpc3.QuadPart) / (double)qpf.QuadPart * 1000;

					if (npointed_count >= pointed_count - 1) {
						store_pointed_image_count++;
						//printf("store_pointed_image_count++\n");
					}
					if (nstoreimage * npointed_count + store_pointed_image_count >= nstorepointedimage) {
						store_pointed_image_count = 0;
						//printf("clear");
					}

				}
				//current = e_current;
				//send_currentset_command(current);

				sleep(7.0);  //20241029 変更 1→
				//cap >> img_phot;
				//flip(img_phot, image_phot, 0);


				//xi_stat = xiGetImage(xiH, 0, &image);
				//t_imagenframe = image.nframe;

				////カメラからイメージ取得
				//while (t_imagenframe + 1 >= image.nframe) {
				//	xi_stat = xiGetImage(xiH, 0, &image);
				//}

			}
			t_REC_switch = REC_switch;
			//if (GetAsyncKeyState(0x52)) REC_switch = 1; // RボタンでRECORDINGスタート

			if (REC_switch == 1 && store_image_count == 0) break;

			//man1の自動キャリブレーション（1&Cボタン）
			if (GetAsyncKeyState(0x31) && GetAsyncKeyState(0x43)) {
				get_man_calib_pettern(objectPoints1, imagePoints1, num_x, num_y, num_z, dx, dy, dz, dWD, hDevice_RU);
				//send_man_manual_command;
				sleep(1000);
				//recieve_man_command();
				man1_flag = point_number;
			}

			//man2の自動キャリブレーション（2&Cボタン）
			if (GetAsyncKeyState(0x32) && GetAsyncKeyState(0x43)) {
				get_man_calib_pettern(objectPoints2, imagePoints2, num_x, num_y, num_z, dx, dy, dz, dWD, hDevice_LU);
				//send_man2_manual_command;
				sleep(1000);
				//recieve_man2_command();
				man2_flag = point_number;
			}

			//man3の自動キャリブレーション (3&Cボタン)
			if (GetAsyncKeyState(0x31) && GetAsyncKeyState(0x43)) {
				get_man_calib_pettern(objectPoints3, imagePoints3, num_x, num_y, num_z, dx, dy, dz, dWD, hDevice_RD);
				//send_man_manual_command;
				sleep(1000);
				//recieve_man_command();
				man3_flag = point_number;
			}

			//man4の自動キャリブレーション（4&Cボタン）
			if (GetAsyncKeyState(0x32) && GetAsyncKeyState(0x43)) {
				get_man_calib_pettern(objectPoints4, imagePoints4, num_x, num_y, num_z, dx, dy, dz, dWD, hDevice_LD);
				//send_man2_manual_command;
				sleep(1000);
				//recieve_man2_command();
				man4_flag = point_number;
			}

			////man1の座標を取得後、pnp問題を解く calib用
	  //      if (man1_flag == point_number) {		//外部パラメータ求める方
		 // 	//f_k = (fObETOL + fTL) * 1000 / pixelpitch;
		 // 		f_k = (fObETOL + fTL) * 1000 / pixelpitch * magnification;
		 // 		K = (Mat_<double>(3, 3) << f_k, 0, cx, 0, f_k, cy, 0, 0, 1);
		 // 	
		 // 		solvePnP(objectPoints1, imagePoints1, K, distortion, rvec1, tvec1);	//外部パラメータ求める方

		 // 		cout << rvec1.at<double>(0, 0) << ", " << rvec1.at<double>(1, 0) << ", " << rvec1.at<double>(2, 0) << endl;
		 // 		cout << tvec1.at<double>(0, 0) << ", " << tvec1.at<double>(1, 0) << ", " << tvec1.at<double>(2, 0) << endl;
		 // 		Rodrigues(rvec1, rmat1);
		 // 		man1_flag = point_number + 2;

		 //   }

			////man3の座標を取得後、pnp問題を解く calib用
			//if (man3_flag == point_number) {		//外部パラメータ求める方
			////f_k = (fObETOL + fTL) * 1000 / pixelpitch;
			//	f_k = (fObETOL + fTL) * 1000 / pixelpitch * magnification;
			//	K = (Mat_<double>(3, 3) << f_k, 0, cx, 0, f_k, cy, 0, 0, 1);

			//	solvePnP(objectPoints3, imagePoints3, K, distortion, rvec3, tvec3);	//外部パラメータ求める方

			//	cout << rvec3.at<double>(0, 0) << ", " << rvec3.at<double>(1, 0) << ", " << rvec3.at<double>(2, 0) << endl;
			//	cout << tvec3.at<double>(0, 0) << ", " << tvec3.at<double>(1, 0) << ", " << tvec3.at<double>(2, 0) << endl;
			//	Rodrigues(rvec3, rmat3);
			//	man3_flag = point_number + 2;

			//}


			//man1の座標を取得後、pnp問題を解く
			if (man1_flag == 0) {		//外部パラメータ求める方
			//f_k = (fObETOL + fTL) * 1000 / pixelpitch;
				f_k = (fObETOL + fTL) * 1000 / pixelpitch * magnification;
				K = (Mat_<double>(3, 3) << f_k, 0, cx, 0, f_k, cy, 0, 0, 1);

				//solvePnP(objectPoints1, imagePoints1, K, distortion, rvec1, tvec1);	//外部パラメータ求める方

				//cout << rvec1.at<double>(0, 0) << ", " << rvec1.at<double>(1, 0) << ", " << rvec1.at<double>(2, 0) << endl;
				//cout << tvec1.at<double>(0, 0) << ", " << tvec1.at<double>(1, 0) << ", " << tvec1.at<double>(2, 0) << endl;
				Rodrigues(rvec1, rmat1);
				man1_flag = point_number + 2;

			}

			//man2の座標を取得後、pnp問題を解く　calib用
			if (man2_flag == 0) {	//外部パラメータ求める方
				f_k = (fObETOL + fTL) * 1000 / pixelpitch * magnification;
				//f_k = (220) * 1000 / pixelpitch * magnification;
				//cout << fObETOL << "  " << fTL << endl;
				//f_k = (fObETOL + fTL) * 1000 / pixelpitch;
				K = (Mat_<double>(3, 3) << f_k, 0, cx, 0, f_k, cy, 0, 0, 1);

				//solvePnP(objectPoints2, imagePoints2, K, distortion, rvec2, tvec2);	//外部パラメータ求める方
				//cout << rvec2.at<double>(0, 0) << ", " << rvec2.at<double>(1, 0) << ", " << rvec2.at<double>(2, 0) << endl;
				//cout << tvec2.at<double>(0, 0) << ", " << tvec2.at<double>(1, 0) << ", " << tvec2.at<double>(2, 0) << endl;

				Rodrigues(rvec2, rmat2);//「光軸方向にスキャン」でいるらしい（３次元用）
				//printf("pnp2\n");
				man2_flag = point_number + 2;
			}

			//man3の座標を取得後、pnp問題を解く calib用
			if (man3_flag == 0) {		//外部パラメータ求める方
			//f_k = (fObETOL + fTL) * 1000 / pixelpitch;
				f_k = (fObETOL + fTL) * 1000 / pixelpitch * magnification;
				K = (Mat_<double>(3, 3) << f_k, 0, cx, 0, f_k, cy, 0, 0, 1);

				//solvePnP(objectPoints3, imagePoints3, K, distortion, rvec3, tvec3);	//外部パラメータ求める方

				//cout << rvec3.at<double>(0, 0) << ", " << rvec3.at<double>(1, 0) << ", " << rvec3.at<double>(2, 0) << endl;
				//cout << tvec3.at<double>(0, 0) << ", " << tvec3.at<double>(1, 0) << ", " << tvec3.at<double>(2, 0) << endl;
				Rodrigues(rvec3, rmat3);
				man3_flag = point_number + 2;

			}

			//man4の座標を取得後、pnp問題を解く　calib用
			if (man4_flag == 0) {	//外部パラメータ求める方
				f_k = (fObETOL + fTL) * 1000 / pixelpitch * magnification;
				//f_k = (220) * 1000 / pixelpitch * magnification;
				//cout << fObETOL << "  " << fTL << endl;
				//f_k = (fObETOL + fTL) * 1000 / pixelpitch;
				K = (Mat_<double>(3, 3) << f_k, 0, cx, 0, f_k, cy, 0, 0, 1);

				//solvePnP(objectPoints4, imagePoints4, K, distortion, rvec4, tvec4);	//外部パラメータ求める方
				//cout << rvec4.at<double>(0, 0) << ", " << rvec4.at<double>(1, 0) << ", " << rvec4.at<double>(2, 0) << endl;
				//cout << tvec4.at<double>(0, 0) << ", " << tvec4.at<double>(1, 0) << ", " << tvec4.at<double>(2, 0) << endl;

				Rodrigues(rvec4, rmat4);//「光軸方向にスキャン」でいるらしい（３次元用）
				printf("pnp4\n");
				man4_flag = point_number + 2;
			}

			//		//man1を光軸方向にスキャン（1&shiftボタン）
			////if (man1_flag == point_number + 1 && GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(0x31)) {
			//		if (man1_flag == point_number + 1 && GetAsyncKeyState(0x10) && GetAsyncKeyState(0x31)) {
			//			//if (man1_flag == point_number + 1 && GetAsyncKeyState(0x44) && GetAsyncKeyState(0x31)) {
			//			man1_zc0 = rmat1.at<double>(0, 2) * man1_pos3D.at<double>(0, 0) + rmat1.at<double>(1, 2) * man1_pos3D.at<double>(0, 1) + rmat1.at<double>(2, 2) * man1_pos3D.at<double>(0, 2) + tvec1.at<double>(2, 0);
			//
			//			s_dWD2 = s_dWD2 - 0.2;
			//			dWD = WD - s_WD + s_dWD2;
			//			calculate_etl2(dWD);
			//			if (current > max_current) current = max_current;
			//			else if (current < 0) current = 0;
			//			send_currentset_command(current);
			//
			//			sleep(50);
			//
			//			for (int i = 0; i < 400; i++) {
			//				////画像取得
			//				//xi_stat = xiGetImage(xiH, 0, &image);
			//				//t_imagenframe = image.nframe;
			//
			//				//while (t_imagenframe == image.nframe) {
			//				//	xi_stat = xiGetImage(xiH, 0, &image);
			//				//}
			//
			//				//Mat calib_img(IMG_HEIGHT2, IMG_WIDTH2, CV_8UC1);
			//				//calib_img.data = (uchar*)image.bp;
			//				//flip(calib_img, calib_img, 0);
			//
			//				Mat calib_img;
			//				cap >> calib_img;
			//
			//				//焦点距離変化
			//				s_dWD2 = s_dWD2 + 0.001;
			//				dWD = WD - s_WD + s_dWD2;
			//				calculate_etl2(dWD);
			//				//cout << "current" << current << endl;
			//				//cout << "max_current" << max_current << endl;
			//				if (current > max_current) current = max_current;
			//				else if (current < 0) current = 0;
			//				send_currentset_command(current);
			//
			//				sleep(20);
			//
			//				//double man1_edge = EdgeCaluculation(calib_img, 324, 244, 20);
			//				double man1_edge = EdgeCaluculation(calib_img, 623, 400, 20);
			//				//i_file << man1_edge << endl;
			//				//i_file << i << "," << man1_edge << endl;
			//
			//				if (man1_edge > man1_edge_max) {
			//					man1_edge_max = man1_edge;
			//					man1_s_dWD2 = s_dWD2;
			//				}
			//
			//				imshow("calib_img", calib_img);
			//				waitKey(1);
			//				//ostringstream oss;
			//				//oss << setfill('0') << setw(3) << i;
			//				//imwrite("injectionpipette_img\\img" + oss.str() + ".png", calib_img);
			//
			//			}
			//
			//			//焦点距離変化
			//			s_dWD2 = s_dWD_0;
			//			dWD = WD - s_WD + s_dWD2;
			//			calculate_etl2(dWD);
			//			if (current > max_current) current = max_current;
			//			else if (current < 0) current = 0;
			//			send_currentset_command(current);
			//
			//			sleep(30);
			//			man1_flag++;
			//			man1_bool = 1;
			//		}

					//man2を光軸方向にスキャン（2&shiftボタン）
					////if (man2_flag == point_number + 1 && GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(0x32)) {
					//if (man2_flag == point_number + 1 && GetAsyncKeyState(0x10) && GetAsyncKeyState(0x32)) {
					//	//if (man2_flag == point_number + 1 && GetAsyncKeyState(0x44) && GetAsyncKeyState(0x32)) {

					//	man2_zc0 = rmat2.at<double>(0, 2) * man2_pos3D.at<double>(0, 0) + rmat2.at<double>(1, 2) * man2_pos3D.at<double>(0, 1) + rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);

					//	s_dWD2 = s_dWD2 - 0.2;
					//	dWD = WD - s_WD + s_dWD2;
					//	calculate_etl2(dWD);

					//	if (current > max_current) current = max_current;
					//	else if (current < 0) current = 0;
					//	send_currentset_command(current);

					//	sleep(50);

					//	for (int i = 0; i < 400; i++) {
					//		//画像取得
					//		//xi_stat = xiGetImage(xiH, 0, &image);
					//		//t_imagenframe = image.nframe;

					//		//while (t_imagenframe == image.nframe) {
					//		//	xi_stat = xiGetImage(xiH, 0, &image);
					//		//}

					//		//Mat calib_img(IMG_HEIGHT2, IMG_WIDTH2, CV_8UC1);
					//		//calib_img.data = (uchar*)image.bp;
					//		//flip(calib_img, calib_img, 0);

					//		Mat calib_img;
					//		cap >> calib_img;

					//		//焦点距離変化
					//		s_dWD2 = s_dWD2 + 0.001;
					//		dWD = WD - s_WD + s_dWD2;
					//		calculate_etl2(dWD);
					//		//cout << "current" << current << endl;
					//		//cout << "max_current" << max_current << endl;
					//		if (current > max_current) current = max_current;
					//		else if (current < 0) current = 0;
					//		send_currentset_command(current);

					//		sleep(20);

					//		//double man2_edge = EdgeCaluculation(calib_img, 324, 244, 100);
					//		double man2_edge = EdgeCaluculation(calib_img, 623, 400, 100);

					//		if (man2_edge > man2_edge_max) {
					//			man2_edge_max = man2_edge;
					//			man2_s_dWD2 = s_dWD2;
					//		}

					//		imshow("calib_img", calib_img);
					//		waitKey(1);
					//		//ostringstream oss;
					//		//oss << setfill('0') << setw(3) << i;
					//		//imwrite("holdingpipette_img\\img" + oss.str() + ".png", calib_img);

					//	}

					//	//焦点距離変化
					//	s_dWD2 = s_dWD_0;
					//	dWD = WD - s_WD + s_dWD2;
					//	calculate_etl2(dWD);
					//	if (current > max_current) current = max_current;
					//	else if (current < 0) current = 0;
					//	send_currentset_command(current);

					//	sleep(30);
					//	man2_flag++;
					//	man2_bool = 1;
					//}

					//視野拡張上のクリックした位置の座標を取得と表示
					//mouseParam mouse;
					//setMouseCallback(window_name, CallBackFunc, &mouse);
					//if (GetAsyncKeyState(0x4B) && mouse.event == EVENT_LBUTTONDOWN) {
					//	t_2Dpos_ex[0] = mouse.x;
					//	t_2Dpos_ex[1] = mouse.y;
					//	cout << "	t_2Dpos_exx = "<<t_2Dpos_ex[0] << endl;
					//	cout << "	t_2Dpos_exy = " << t_2Dpos_ex[1] << endl;
					//	circle(circle_img, Point((int)t_2Dpos_ex[0], (int)t_2Dpos_ex[1]), 2, Scalar(255, 255, 0), 5);//変換確認用
					//	sleep(500);
					//}

					//man1の３次元座標をカメラ座標系に変換
			if (man1_flag == point_number + 2) {
				projectPoints(man1_pos3D, rvec1, tvec1, K, distortion, man1_pos2D);
				//cout <<"man1_3D" << man1_pos3D << endl;
				//cout <<"man1_2D"<< man1_pos2D << endl;
				//cout <<"rvec1"<< rvec1 << endl;
				//cout <<"tvec"<< tvec1 << endl;
				//cout <<"K1"<< K << endl;
				//cout <<"distortion1"<< distortion << endl;

				//temp3_man1_pos2D[0] = temp2_man1_pos2D[0];//３ループ前のデータを保存
				//temp3_man1_pos2D[1] = temp2_man1_pos2D[1];//３ループ前のデータを保存

				//temp2_man1_pos2D[0] = temp1_man1_pos2D[0];//２ループ前のデータを保存
				//temp2_man1_pos2D[1] = temp1_man1_pos2D[1];//２ループ前のデータを保存

				//temp1_man1_pos2D[0] = man1_cpos[0];//１ループ前のデータを保存
				//temp1_man1_pos2D[1] = man1_cpos[1];//１ループ前のデータを保存

				//past_man1_cpos[0] = man1_cpos[0];//１ループ前のデータを保存
				//past_man1_cpos[1] = man1_cpos[1];//１ループ前のデータを保存
				//past_man1_cpos[2] = man1_cpos[2];//１ループ前のデータを保存

				man1_cpos[0] = man1_pos2D.at<double>(0, 0) + man1_dx;
				man1_cpos[1] = man1_pos2D.at<double>(0, 1) + man1_dy;
				man1_zc = rmat1.at<double>(0, 2) * man1_pos3D.at<double>(0, 0) + rmat1.at<double>(1, 2) * man1_pos3D.at<double>(0, 1) + rmat1.at<double>(2, 2) * man1_pos3D.at<double>(0, 2) + tvec1.at<double>(2, 0);
				//man1_cpos[2] = (man1_zc - man1_zc0 + (man1_s_dWD2 - s_dWD_0) * 1000) / pixelpitch;
				man1_cpos[2] = (man1_zc - man1_zc0 + (man1_s_dWD2 - s_dWD_0) * 1000) / pixelpitch * magnification;
				double man1_zc2 = man1_zc * 1000 / pixelpitch * magnification;
				//printf("x,y=%f  ,%f\n", man1_cpos[0], man1_cpos[1]);
				//printf("cam1(zc,zc0,dWD,dWD0)=(%f,%f,%f,%f)\n", man1_zc, man1_zc0, man1_s_dWD2, s_dWD_0);
				//printf("cam(x1,y1,z1,z1_2,i)=(%f,%f,%f,%f,%d)\n", man1_cpos[0], man1_cpos[1], man1_cpos[2], man1_zc2, man1_flag);
				//cout << "point_man = " << point_man_x << "," << point_man_y << endl;
			}

			//man2の３次元座標をカメラ座標系に変換
			if (man2_flag == point_number + 2) {
				//cout << rvec2 << endl;
				//cout << tvec2 << endl;
				//cout << K << endl;
				//cout << distortion << endl;

				projectPoints(man2_pos3D, rvec2, tvec2, K, distortion, man2_pos2D);
				//cout << "man2_3D" << man2_pos3D << endl;
				//cout << "man2_2D" << man2_pos2D << endl;
				//cout << "rvec2" << rvec2 << endl;
				//cout << "tvec2" << tvec2 << endl;
				//cout << "K2" << K << endl;
				//cout << "distortion2" << distortion << endl;
				//cout << man2_pos2D << endl;
				man2_cpos[0] = man2_pos2D.at<double>(0, 0) + man2_dx;
				man2_cpos[1] = man2_pos2D.at<double>(0, 1) + man2_dy;
				//man2_zc = rmat2.at<double>(0, 2) * man2_pos3D.at<double>(0, 0) + rmat2.at<double>(1, 2) * man2_pos3D.at<double>(0, 1) + rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);
				man2_zc = rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);//ピペットをｘ－ｙ平面上で動かすとｚ座標が変わったしまったため、応急処置。もともとのコードは→//man2_zc = rmat2.at<double>(0, 2) * man2_pos3D.at<double>(0, 0) + rmat2.at<double>(1, 2) * man2_pos3D.at<double>(0, 1) + rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);
				//man2_cpos[2] = (man2_zc - man2_zc0 + (man2_s_dWD2 - s_dWD_0) * 1000) / pixelpitch;
				man2_cpos[2] = (man2_zc - man2_zc0 + (man2_s_dWD2 - s_dWD_0) * 1000) / pixelpitch * magnification;
				double man2_zc2 = man2_zc * 1000 / pixelpitch * magnification;
				//printf("x,y=(%f  ,%f\n", man2_cpos[0], man2_cpos[1]);
				//printf("cam2(zc,zc0,dWD,dWD0)=(%f,%f,%f,%f)\n", man2_zc, man2_zc0, man2_s_dWD2, s_dWD_0);
				//printf("cam(x2,y2,z2,z2_2,i)=(%f,%f,%f,%f,%d)\n", man2_cpos[0], man2_cpos[1], man2_cpos[2], man2_zc2, man2_flag);
			}

			//man3の３次元座標をカメラ座標系に変換
			if (man3_flag == point_number + 2) {
				//cout << rvec3 << endl;
				//cout << tvec3 << endl;
				//cout << K << endl;
				//cout << distortion << endl;

				projectPoints(man3_pos3D, rvec3, tvec3, K, distortion, man3_pos2D);
				//cout << "man2_3D" << man2_pos3D << endl;
				//cout << "man2_2D" << man2_pos2D << endl;
				//cout << "rvec2" << rvec2 << endl;
				//cout << "tvec2" << tvec2 << endl;
				//cout << "K2" << K << endl;
				//cout << "distortion2" << distortion << endl;
				//cout << man2_pos2D << endl;
				man3_cpos[0] = man3_pos2D.at<double>(0, 0) + man3_dx;
				man3_cpos[1] = man3_pos2D.at<double>(0, 1) + man3_dy;
				//man2_zc = rmat2.at<double>(0, 2) * man2_pos3D.at<double>(0, 0) + rmat2.at<double>(1, 2) * man2_pos3D.at<double>(0, 1) + rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);
				man3_zc = rmat3.at<double>(2, 2) * man3_pos3D.at<double>(0, 2) + tvec3.at<double>(2, 0);//ピペットをｘ－ｙ平面上で動かすとｚ座標が変わったしまったため、応急処置。もともとのコードは→//man2_zc = rmat2.at<double>(0, 2) * man2_pos3D.at<double>(0, 0) + rmat2.at<double>(1, 2) * man2_pos3D.at<double>(0, 1) + rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);
				//man2_cpos[2] = (man2_zc - man2_zc0 + (man2_s_dWD2 - s_dWD_0) * 1000) / pixelpitch;
				man3_cpos[2] = (man3_zc - man3_zc0 + (man3_s_dWD2 - s_dWD_0) * 1000) / pixelpitch * magnification;
				double man3_zc2 = man3_zc * 1000 / pixelpitch * magnification;
				//printf("x,y=(%f  ,%f\n", man2_cpos[0], man2_cpos[1]);
				//printf("cam2(zc,zc0,dWD,dWD0)=(%f,%f,%f,%f)\n", man2_zc, man2_zc0, man2_s_dWD2, s_dWD_0);
				//printf("cam(x2,y2,z2,z2_2,i)=(%f,%f,%f,%f,%d)\n", man2_cpos[0], man2_cpos[1], man2_cpos[2], man2_zc2, man2_flag);
			}

			//man4の３次元座標をカメラ座標系に変換
			if (man4_flag == point_number + 2) {
				//cout << rvec2 << endl;
				//cout << tvec2 << endl;
				//cout << K << endl;
				//cout << distortion << endl;

				projectPoints(man4_pos3D, rvec4, tvec4, K, distortion, man4_pos2D);
				//cout << "man2_3D" << man2_pos3D << endl;
				//cout << "man2_2D" << man2_pos2D << endl;
				//cout << "rvec2" << rvec2 << endl;
				//cout << "tvec2" << tvec2 << endl;
				//cout << "K2" << K << endl;
				//cout << "distortion2" << distortion << endl;
				//cout << man2_pos2D << endl;
				man4_cpos[0] = man4_pos2D.at<double>(0, 0) + man4_dx;
				man4_cpos[1] = man4_pos2D.at<double>(0, 1) + man4_dy;
				//man2_zc = rmat2.at<double>(0, 2) * man2_pos3D.at<double>(0, 0) + rmat2.at<double>(1, 2) * man2_pos3D.at<double>(0, 1) + rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);
				man4_zc = rmat4.at<double>(2, 2) * man4_pos3D.at<double>(0, 2) + tvec4.at<double>(2, 0);//ピペットをｘ－ｙ平面上で動かすとｚ座標が変わったしまったため、応急処置。もともとのコードは→//man2_zc = rmat2.at<double>(0, 2) * man2_pos3D.at<double>(0, 0) + rmat2.at<double>(1, 2) * man2_pos3D.at<double>(0, 1) + rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);
				//man2_cpos[2] = (man2_zc - man2_zc0 + (man2_s_dWD2 - s_dWD_0) * 1000) / pixelpitch;
				man4_cpos[2] = (man4_zc - man4_zc0 + (man4_s_dWD2 - s_dWD_0) * 1000) / pixelpitch * magnification;
				double man4_zc2 = man4_zc * 1000 / pixelpitch * magnification;
				//printf("x,y=(%f  ,%f\n", man2_cpos[0], man2_cpos[1]);
				//printf("cam2(zc,zc0,dWD,dWD0)=(%f,%f,%f,%f)\n", man2_zc, man2_zc0, man2_s_dWD2, s_dWD_0);
				//printf("cam(x2,y2,z2,z2_2,i)=(%f,%f,%f,%f,%d)\n", man2_cpos[0], man2_cpos[1], man2_cpos[2], man2_zc2, man2_flag);
			}

			//man2の奥行き座標をman1に一致させる（3&shiftボタン）
			if (man2_flag >= point_number + 1 && GetAsyncKeyState(0x10) && GetAsyncKeyState(0x33)) {
				man2_zc0 = rmat2.at<double>(0, 2) * man2_pos3D.at<double>(0, 0) + rmat2.at<double>(1, 2) * man2_pos3D.at<double>(0, 1) + rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);
				//man2_s_dWD2 = man1_s_dWD2;
				man2_s_dWD2 = man1_s_dWD2 + (man1_zc - man1_zc0 - (man2_zc - man2_zc0)) / 1000;
				//man2_s_dWD2 = man1_s_dWD2 + (man1_zc - man1_zc0 - (man2_zc - man2_zc0) - (man2_delta_f1 - delta_f0)) / 1000;
				man2_flag = point_number + 2;
				man2_bool = 1;
			}

			//座標変換と逆変換の誤差を埋める(ホールディング1　1&ctrl)
			if (GetAsyncKeyState(0x31) && GetAsyncKeyState(0xA2))
			{
				Getposition(hDevice_LU, man2_xw, man2_yw, man2_zw);
				man2_pos3D.at<double>(0, 0) = man2_xw;
				man2_pos3D.at<double>(0, 1) = -man2_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
				man2_pos3D.at<double>(0, 2) = man2_zw;//三次元時
				printf("cam(x2,y2,z2,z2_2,i)=(%f,%f,%f,%f,%d)\n", man2_cpos[0], man2_cpos[1], man2_cpos[2], man2_zc, man2_flag);
				camera_to_world1(man2_cpos, temp_man2_pos3D);
				CtoW_man2_dx = man2_pos3D.at<double>(0, 0) - temp_man2_pos3D[0];
				CtoW_man2_dy = man2_pos3D.at<double>(0, 1) - temp_man2_pos3D[1];
				CtoW_man2_dz = man2_pos3D.at<double>(0, 2) - temp_man2_pos3D[2];
				//std::cout << "man2_pos2Dx=" << man2_pos2D.at<double>(0, 0) << std::endl;
				//std::cout << "man2_pos2Dy=" << man2_pos2D.at<double>(0, 1) << std::endl;
				//std::cout << "man2_pos2Dz=" << man2_pos2D.at<double>(0, 2) << std::endl;
				//std::cout << "man2_pos3Dx=" << man2_pos3D.at<double>(0, 0) << std::endl;
				//std::cout << "man2_pos3Dy=" << man2_pos3D.at<double>(0, 1) << std::endl;
				//std::cout << "man2_pos3Dz=" << man2_pos3D.at<double>(0, 2) << std::endl;
				//std::cout << "temp_man2_pos3Dx=" << temp_man2_pos3D[0] << std::endl;
				//std::cout << "temp_man2_pos3Dy=" << temp_man2_pos3D[1] << std::endl;
				//std::cout << "temp_man2_pos3Dz=" << temp_man2_pos3D[2] << std::endl;
				//std::cout << "CtoW_man2_dx=" << CtoW_man2_dx << std::endl;
				//std::cout << "CtoW_man2_dy=" << CtoW_man2_dy << std::endl;
				//std::cout << "CtoW_man2_dz=" << CtoW_man2_dz << std::endl;
				sleep(500);
			}

			//座標変換と逆変換の誤差を埋める(ホールディング2　2&ctrl)
			if (GetAsyncKeyState(0x32) && GetAsyncKeyState(0xA2))
			{
				Getposition(hDevice_LD, man4_xw, man4_yw, man4_zw);
				man4_pos3D.at<double>(0, 0) = man4_xw;
				man4_pos3D.at<double>(0, 1) = -man4_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
				man4_pos3D.at<double>(0, 2) = man4_zw;//三次元時
				printf("cam(x2,y2,z2,z2_2,i)=(%f,%f,%f,%f,%d)\n", man4_cpos[0], man4_cpos[1], man4_cpos[2], man4_zc, man4_flag);
				camera_to_world2(man4_cpos, temp_man4_pos3D);
				CtoW_man4_dx = man4_pos3D.at<double>(0, 0) - temp_man4_pos3D[0];
				CtoW_man4_dy = man4_pos3D.at<double>(0, 1) - temp_man4_pos3D[1];
				CtoW_man4_dz = man4_pos3D.at<double>(0, 2) - temp_man4_pos3D[2];
				//std::cout << "man4_pos2Dx=" << man4_pos2D.at<double>(0, 0) << std::endl;
				//std::cout << "man4_pos2Dy=" << man4_pos2D.at<double>(0, 1) << std::endl;
				//std::cout << "man4_pos2Dz=" << man4_pos2D.at<double>(0, 2) << std::endl;
				//std::cout << "man4_pos3Dx=" << man4_pos3D.at<double>(0, 0) << std::endl;
				//std::cout << "man4_pos3Dy=" << man4_pos3D.at<double>(0, 1) << std::endl;
				//std::cout << "man4_pos3Dz=" << man4_pos3D.at<double>(0, 2) << std::endl;
				//std::cout << "temp_man4_pos3Dx=" << temp_man4_pos3D[0] << std::endl;
				//std::cout << "temp_man4_pos3Dy=" << temp_man4_pos3D[1] << std::endl;
				//std::cout << "temp_man4_pos3Dz=" << temp_man4_pos3D[2] << std::endl;
				//std::cout << "CtoW_man4_dx=" << CtoW_man4_dx << std::endl;
				//std::cout << "CtoW_man4_dy=" << CtoW_man4_dy << std::endl;
				//std::cout << "CtoW_man4_dz=" << CtoW_man4_dz << std::endl;
				sleep(500);
			}

			//拡張画像から通常視野の座標に変換
			t_2Dpos[0] = ex_to_normalx(t_2Dpos_ex[0]);
			t_2Dpos[1] = ex_to_normaly(t_2Dpos_ex[1]);
			//t_2Dpos[1] = -t_2Dpos[1];
			//std::cout << "t_2Dposx=" << t_2Dpos[0] << std::endl;
			//std::cout << "t_2Dposy=" << t_2Dpos[1] << std::endl;

			//通常視野から拡張画像に変換
			//man4_cpos_ex[0] = 623.0 + (173.0 / 623.0) * (man4_cpos[0] - 623.0);
			//man4_cpos_ex[1] = 504.0 + (144.0 / 504.0) * (man4_cpos[1] - 504.0);
			//man1_cpos_ex[0] = 623.0 + (173.0 / 623.0) * (man1_cpos[0] - 623.0);
			//man1_cpos_ex[1] = 504.0 + (144.0 / 504.0) * (man1_cpos[1] - 504.0);
			//man2_cpos_ex[0] = 623.0 + (173.0 / 623.0) * (man2_cpos[0] - 623.0);
			//man2_cpos_ex[1] = 504.0 + (144.0 / 504.0) * (man2_cpos[1] - 504.0);
			man4_cpos_ex[0] = normal_to_exx(man4_cpos[0]);
			man4_cpos_ex[1] = normal_to_exy(man4_cpos[1]);
			man1_cpos_ex[0] = normal_to_exx(man1_cpos[0]);
			man1_cpos_ex[1] = normal_to_exy(man1_cpos[1]);
			man2_cpos_ex[0] = normal_to_exx(man2_cpos[0]);
			man2_cpos_ex[1] = normal_to_exy(man2_cpos[1]);
			man3_cpos_ex[0] = normal_to_exx(man3_cpos[0]);
			man3_cpos_ex[1] = normal_to_exy(man3_cpos[1]);
			//point_man_x = man1_cpos_ex[0];
			//point_man_y = man1_cpos_ex[1];

			////保持判定　確認 Rボタン
			//if (GetAsyncKeyState(0x52)) {
			//	if (GetAsyncKeyState(0x32)) {
			//		check_inj1(hold_flag2);
			//		cout << "hold_flag2 ==" << hold_flag2 << endl;
			//	}
			//	else if (GetAsyncKeyState(0x34)) {
			//		check_inj2(hold_flag4);
			//		cout << "hold_flag4 ==" << hold_flag4 << endl;
			//	}
			//	sleep(500);
			//}

			////スリットの位置を取得
			//if (GetAsyncKeyState(0x52) && GetAsyncKeyState(0x32)) {
			//	if (inside_flag == 0) {
			//		Get_circle_size(C_radius, C_center_x, C_center_y, man2_cpos_ex);
			//		C_radius_ex = C_radius * radius_ratio;
			//		inside_flag = 1;
			//		cout << "C_radius=" << C_radius << endl;
			//		cout << "C_center_x=" << C_center_x << endl;
			//		cout << "C_center_y=" << C_center_y << endl;
			//	}
			//	else if (inside_flag > 0) {
			//		inside_flag = 0;
			//	}

			//	cout << "inside_flag=" << inside_flag << endl;

			//	sleep(100);
			//}
			//else if (GetAsyncKeyState(0x52) && GetAsyncKeyState(0x34)) {
			//	if (inside_flag == 0) {
			//		Get_circle_size(C_radius, C_center_x, C_center_y, man4_cpos_ex);
			//		C_radius_ex = C_radius * radius_ratio;
			//		inside_flag = 1;
			//		cout << "C_radius=" << C_radius << endl;
			//		cout << "C_center_x=" << C_center_x << endl;
			//		cout << "C_center_y=" << C_center_y << endl;
			//	}
			//	else if (inside_flag > 0) {
			//		inside_flag = 0;
			//	}
			//	sleep(100);
			//}

			//int MaxID = 0;

			////スリットの位置を取得(萩原AI)
			//if (GetAsyncKeyState(0x52) && GetAsyncKeyState(0x35)) {
			//	if (inside_flag == 0) {
			//		man2_cpos_c[0] = (IMG_WIDTH * 0.4) + (Kc * (man2_cpos_ex[0] - point_man_x_s));
			//		man2_cpos_c[1] = (IMG_HEIGHT * 0.4) + (Kc * (man2_cpos_ex[1] - point_man_y_s));
			//		transparency();
			//		
			//		//最大の領域のみ抽出
			//		Mat Labelmg;
			//		Mat stats;
			//		Mat centroids;
			//		int nLab = connectedComponentsWithStats(bg, Labelmg, stats, centroids);
			//		int Max = 0;

			//		cout << "nLab =" << nLab << endl;

			//		for (int i = 1; i < nLab; ++i) {//全てのラベルを回す
			//			int* param = stats.ptr<int>(i);
			//			if (Max < param[ConnectedComponentsTypes::CC_STAT_AREA]) {//最大値より大きければ
			//				Max = param[ConnectedComponentsTypes::CC_STAT_AREA];//最大値を更新
			//				MaxID = i;
			//				trimmed_area = Max;

			//			}
			//		}

			//		int* param = stats.ptr<int>(MaxID);
			//		int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			//		int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			//		int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
			//		int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
			//		C_center_x = x + width / 2;
			//		C_center_y = y + height / 2;
			//		C_radius_c = (width + height) / 4.0;
			//		cout << "Ccenter =" << C_center_x << "," << C_center_y << endl;
			//		cout << "C_radius=" << C_radius_c << endl;
			//		cout << "Max_ID =" << MaxID << endl;
			//		sleep(100);
			//		inside_flag = 3;
			//	}
			//	else if (inside_flag > 0) {
			//		inside_flag = 0;
			//	}
			//	sleep(100);
			//}

			//体細胞挿入支援
			//if (inside_flag == 1) {
			//	if (pow(man1_cpos_ex[0] - C_center_x, 2) + pow(man1_cpos_ex[1] - C_center_y, 2) < pow(C_radius_ex, 2) && man1_cpos_ex[0] > C_center_x) {
			//		slit_x = man1_cpos_ex[0];
			//		slit_y = man1_cpos_ex[1];
			//		cout << "slit_pos=" << slit_x << "," << slit_y << endl;
			//		inside_flag = 2;
			//	}
			//	cv::circle(t_frame, cv::Point(C_center_x, C_center_y), (int)C_radius_ex, cv::Scalar(0, 0, 255), 2);
			//	//sleep(1);
			//}
			//else if (inside_flag == 2) {
			//	if (pow(man1_cpos_ex[0] - C_center_x, 2) + pow(man1_cpos_ex[1] - C_center_y, 2) > pow(C_radius_ex, 2) && man1_cpos_ex[0] > C_center_x) inside_flag = 1;
			//	else cout << "inside" << endl;
			//	cv::circle(t_frame, cv::Point(C_center_x, C_center_y), (int)C_radius_ex, cv::Scalar(0, 0, 255), 2);
			//	//sleep(1);
			//}
			//else if (inside_flag == 3) {
			//	if (bg.at<int>(man1_cpos_c[1], man1_cpos_c[0]) == MaxID) {
			//		slit_x = man1_cpos_ex[0];
			//		slit_y = man1_cpos_ex[1];
			//		cout << "slit_pos2=" << slit_x << "," << slit_y << endl;
			//		inside_flag = 4;
			//	}
			//}
			//else if (inside_flag == 4) {
			//	if (pow(man1_cpos_c[0] - C_center_x, 2) + pow(man1_cpos_c[1] - C_center_y, 2) > pow(C_radius_c, 2) && man1_cpos_c[0] > C_center_x) inside_flag = 3;
			//	else cout << "inside2" << endl;
			//	cv::circle(small_img, cv::Point(C_center_x, C_center_y), (int)C_radius_c, cv::Scalar(0, 0, 255), 2);
			//	//sleep(1);
			//}
		}

		if (switch_flag == 2)
		{
			QueryPerformanceCounter(&loop_st);
			//初期ミラー角度に変更
			pData[0] = 32767 / 20 * s_mirror_x + 32767;
			pData[1] = 32767 / 20 * s_mirror_y + 32767;

			//DAの出力
			DaOutputDA(ghDeviceHandle_da1, 2, &gstrDaSmplChReq[0], &pData[0]);

			sleep(0.2);

			cap >> circle_img;

			//flip(circle_img, circle_img, 0);
			namedWindow("circle_img");
			//cv::circle(circle_img, Point((int)t_2Dpos[0], (int)t_2Dpos[1]), 2, Scalar(255, 255, 0), 5);//変換確認用

			//大島追加
			// ↓↓↓ ここからAIロジック (2ストリーム + ROIロック + 5フレームフィルタ) を追加 ↓↓↓
			// 3チャンネルBGRに変換
			Mat frame_bgr_for_ai;
			if (circle_img.channels() == 1) {
				cv::cvtColor(circle_img, frame_bgr_for_ai, cv::COLOR_GRAY2BGR);
			}
			else {
				frame_bgr_for_ai = circle_img.clone();
			}
			if (!frame_bgr_for_ai.empty()){
				// ★ 1. 初期化 (Shift+D が押された瞬間の処理)
				if (g_ai_capture_request.load())
				{
					// ★ 1a. この瞬間の円を検出し、ROIサイズを計算・ロックする
					Mat threshold_img_normal;
					cv::threshold(circle_img, threshold_img_normal, Threshold, 255, THRESH_BINARY_INV);
					double pip_x = man2_cpos[0];
					double pip_y = man2_cpos[1];
					// ピペットをマスキング
					if (pip_x > 0 && pip_x < threshold_img_normal.cols) {
						cv::rectangle(threshold_img_normal,
							cv::Point(0, 0),
							cv::Point(static_cast<int>(pip_x), threshold_img_normal.rows),
							cv::Scalar(0), cv::FILLED);
					}
					//消す
					// ▼▼▼【追加1】二値化の状態を確認するためのウィンドウを表示 ▼▼▼
					// ※ ループ内で cv::waitKey(1) が呼ばれている前提です
					/*cv::imshow("Debug: Threshold", threshold_img_normal);*/
					// ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲

					cv::Mat LabelImg, stats, centroids;
					int nLab = cv::connectedComponentsWithStats(threshold_img_normal, LabelImg, stats, centroids);
					double min_distance_sq = DBL_MAX;
					double detected_radius = 0;
					bool found_circle = false;

					// ▼▼▼【追加2】描画用に選ばれたブロブの矩形情報を保存する変数 ▼▼▼
					cv::Rect best_blob_rect;
					// ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲

					for (int i = 1; i < nLab; ++i) {
						int* param = stats.ptr<int>(i);
						int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
						int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];

						// （デバッグ用）候補になりうるブロブを青い薄い枠で描画してもよい
						// // cv::Rect candidate_rect(param[cv::ConnectedComponentsTypes::CC_STAT_LEFT], param[cv::ConnectedComponentsTypes::CC_STAT_TOP], width, height);
						// cv::rectangle(circle_img, candidate_rect, cv::Scalar(255, 0, 0), 1); // 青色

						if (height < dia_max && height > dia_min && width < dia_max && width > dia_min) {
							double current_center_x = centroids.at<double>(i, 0);
							double current_center_y = centroids.at<double>(i, 1);
							double dist_sq = pow(current_center_x - pip_x, 2) + pow(current_center_y - pip_y, 2);
							if (dist_sq < min_distance_sq) {
								min_distance_sq = dist_sq;
								found_circle = true;
								detected_radius = (width + height) / 4.0;

								// ▼▼▼【追加3】最も近いブロブの座標を保存 ▼▼▼
								best_blob_rect = cv::Rect(
									param[cv::ConnectedComponentsTypes::CC_STAT_LEFT],
									param[cv::ConnectedComponentsTypes::CC_STAT_TOP],
									width, height);
								// ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲

							}
						}
					}
					int CROP_WIDTH, CROP_HEIGHT;
					if (found_circle) {

						// ▼▼▼【追加4】検出された円（ブロブ）を赤枠で囲む ▼▼▼
						cv::rectangle(circle_img, best_blob_rect, cv::Scalar(0, 0, 255), 2); // 赤色: 選ばれた円
						// 検索の中心点（ピペット位置）を緑の点で描画
						cv::circle(circle_img, cv::Point((int)pip_x, (int)pip_y), 5, cv::Scalar(0, 255, 0), -1);
						// ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲

						double detected_diameter = detected_radius * 2.0;
						const double RATIO_DIAMETER = 1.0;
						const double RATIO_HEIGHT = 1.64;
						const double RATIO_WIDTH = 4.1;
						CROP_HEIGHT = static_cast<int>(detected_diameter * (RATIO_HEIGHT / RATIO_DIAMETER));
						CROP_WIDTH = static_cast<int>(detected_diameter * (RATIO_WIDTH / RATIO_DIAMETER));
					}
					else {
						CROP_WIDTH = 480;
						CROP_HEIGHT = 192;
						// 見つからなかった場合、画面に文字を出す
						cv::putText(circle_img, "Circle NOT Found", cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
					}
					// ★ 1b. ROIサイズをグローバル変数に「ロック」する
					g_locked_roi_width.store(CROP_WIDTH);
					g_locked_roi_height.store(CROP_HEIGHT);
					// ★ 1c. AIの内部状態を初期化する
					std::cout << "[AI] Initializing State (Running init_hidden)..." << std::endl;
					try {
						auto state_tuple = g_ai_model.get_method("init_hidden")({ 1 }).toTuple();
						g_ai_hidden_h = state_tuple->elements()[0].toTensor();
						g_ai_hidden_c = state_tuple->elements()[1].toTensor();
						g_ai_progress.store(0.0);
						g_ai_is_stop.store(false);
						g_ai_initial_frame_mat.release(); // ★ 最初のフレームをクリア
						g_ai_stop_counter.store(0);    // ★ 連続STOPカウンターをクリア
						g_ai_motor_engaged.store(true);
						//oshima
						// ★ [変更] カウンタ類の初期化
						g_ai_drive_counter.store(0);
						g_last_pred_class.store(-1);
						// ★ [変更] 開始時は50ステップ移動
						int start_steps = 70;
						g_total_moved_steps.store(start_steps); // 積算ステップに50をセット
						std::thread thread_m1_start(moveStepRelative, std::ref(*motor1), start_steps, g_step_speed, "Motor1");
						thread_m1_start.detach();

						// 正方向に200ステップ移動
						//std::thread thread_m1_start(moveStepRelative, std::ref(*motor1), 100, g_step_speed, "Motor1");
						//thread_m1_start.detach();

						g_ai_capture_request.store(false);
					}
					catch (const c10::Error& e) {
						std::cerr << "[AI] init_hidden error: " << e.what() << std::endl;
					}
				}
				// ★ 2. AI実行中の推論 (毎フレームの処理)
				else if (g_ai_motor_engaged.load())
				{
					LARGE_INTEGER ai_start, ai_end;
					QueryPerformanceCounter(&ai_start);
					try {
						// ★ 2a. 「ロックされた」ROIサイズを使ってトリミング
						const int CROP_WIDTH = g_locked_roi_width.load();
						const int CROP_HEIGHT = g_locked_roi_height.load();
						double pip_x = man2_cpos[0];
						double pip_y = man2_cpos[1];
						int crop_x = static_cast<int>(pip_x);
						int crop_y = static_cast<int>(pip_y - (CROP_HEIGHT / 2.0));
						// (安全チェック)
						if (crop_x < 0) crop_x = 0;
						if (crop_y < 0) crop_y = 0;
						if (crop_x + CROP_WIDTH > frame_bgr_for_ai.cols) {
							crop_x = frame_bgr_for_ai.cols - CROP_WIDTH;
						}
						if (crop_y + CROP_HEIGHT > frame_bgr_for_ai.rows) {
							crop_y = frame_bgr_for_ai.rows - CROP_HEIGHT;
						}
						if (crop_x < 0) crop_x = 0;
						if (crop_y < 0) crop_y = 0;
						ai_roi = cv::Rect(crop_x, crop_y, CROP_WIDTH, CROP_HEIGHT);
						Mat cropped_frame = frame_bgr_for_ai(ai_roi);
						// ★ 2b. 差分フレームを計算
						Mat diff_frame;
						if (g_ai_initial_frame_mat.empty()) {
							g_ai_initial_frame_mat = cropped_frame.clone();
						}
						// [変更] 差分計算: 「現在 - 最初」 の差分を取る
						cv::absdiff(cropped_frame, g_ai_initial_frame_mat, diff_frame);
						// ★ 2c. 2つのフレームを前処理
						torch::Tensor tensor_raw = ai_preprocess(cropped_frame).unsqueeze(0);
						torch::Tensor tensor_diff = ai_preprocess(diff_frame).unsqueeze(0);
						// ★ 2d. "step" メソッドで推論実行
						std::vector<torch::jit::IValue> inputs;
						inputs.push_back(tensor_raw);
						inputs.push_back(tensor_diff);
						inputs.push_back(g_ai_hidden_h);
						inputs.push_back(g_ai_hidden_c);
						// [変更] 出力タプルの構造変更に対応
						// Python側: return (out_cls, out_bnd), (next_h, next_c)
						auto output_tuple = g_ai_model.get_method("step")(inputs).toTuple();

						// 1つ目の要素は (Class, Boundary) のタプル
						auto prediction_tuple = output_tuple->elements()[0].toTuple();
						torch::Tensor cls_logits = prediction_tuple->elements()[0].toTensor(); // クラス分類 [1, 3]
						// torch::Tensor bnd_logits = prediction_tuple->elements()[1].toTensor(); // 境界検知 (今回は使わない)

						// 2つ目の要素は (h, c) のタプル
						auto state_tuple = output_tuple->elements()[1].toTuple();
						g_ai_hidden_h = state_tuple->elements()[0].toTensor();
						g_ai_hidden_c = state_tuple->elements()[1].toTensor();

						// [変更] 3クラス分類の判定 (Argmax)
						// 0: 開始前, 1: 動作中, 2: 完了
						int pred_class = cls_logits.argmax(1).item<int>();
						g_ai_progress.store(static_cast<double>(pred_class)); // 画面表示用に0.0, 1.0, 2.0を入れる

						// [変更] 停止判定ロジック (クラス2が3回続いたら停止)
						const int TARGET_STOP_CLASS = 2;

						//oshima
						// --- ★ ここからロジック変更 ---
						int last_class = g_last_pred_class.load();
						int current_drive_count = 0;
						// クラスが変わったらカウンタをリセット (0->1等の切り替わり時は回転しない)
						if (pred_class != last_class) {
							g_ai_drive_counter.store(1); // 新しいクラスの1回目としてセット
						}
						else {
							// 同じクラスが続いている場合カウントアップ
							current_drive_count = g_ai_drive_counter.load() + 1;
							g_ai_drive_counter.store(current_drive_count);
						}
						// 前回のクラス情報を更新
						g_last_pred_class.store(pred_class);
						// --- クラスごとの動作判定 ---
						if (pred_class == 0) {
							// クラス0: 10回ごとに50ステップ順回転
							if (current_drive_count > 0 && current_drive_count % 50 == 0) {
								int move_val = 15;
								g_total_moved_steps.fetch_add(move_val); // 積算加算
								std::thread t(moveStepRelative, std::ref(*motor1), move_val, g_step_speed, "Motor1");
								t.detach();
							}
							g_ai_stop_counter.store(0); // 停止カウンタはリセット
						}
						else if (pred_class == 1) {
							// クラス1: 10回ごとに25ステップ順回転
							if (current_drive_count > 0 && current_drive_count % 50 == 0) {
								int move_val = 10;
								g_total_moved_steps.fetch_add(move_val); // 積算加算
								std::thread t(moveStepRelative, std::ref(*motor1), move_val, g_step_speed, "Motor1");
								t.detach();
							}
							g_ai_stop_counter.store(0); // 停止カウンタはリセット
						}
						else if (pred_class == 2) {
							// クラス2: 停止判定ロジック
							int current_stop_count = g_ai_stop_counter.load();
							g_ai_stop_counter.store(current_stop_count + 1);
							if (g_ai_stop_counter.load() >= 3) {
								g_ai_is_stop.store(true);
							}
							// クラス2のときは駆動用カウンタはリセットしても維持しても良いが、
							// 「他のクラスになったらリセット」の要件に基づき、
							// 次回0か1が来たときに if(pred_class != last_class) でリセットされるため特段の処理不要
						}

						/*if (pred_class == TARGET_STOP_CLASS) {
							int current_count = g_ai_stop_counter.load();
							g_ai_stop_counter.store(current_count + 1);
							if (g_ai_stop_counter.load() >= 3) {
								g_ai_is_stop.store(true);
							}
						}*/
						else {
							g_ai_stop_counter.store(0);
						}
					}
					catch (const c10::Error& e) {
						// 【修正】エラーログ出力を復活
						std::cerr << "[AI] step error: " << e.what() << std::endl;
					}

					// 【修正】FPS計測をcatchの外に出し、毎回実行するように修正
					QueryPerformanceCounter(&ai_end);
					double ai_elapsed_ms = ((double)ai_end.QuadPart - (double)ai_start.QuadPart) / (double)qpf.QuadPart * 1000.0;
					if (ai_elapsed_ms > 0) g_ai_fps.store(1000.0 / ai_elapsed_ms);
				}
			}
			// ↑↑↑ AIロジックここまで ↑↑↑

			//焦点距離変化（上矢印ボタン）
			if (GetAsyncKeyState(VK_UP)) {
				s_dWD2 = s_dWD2 + 0.005;
				//s_dWD2 = s_dWD2 + 0.3;
				dWD = WD - s_WD + s_dWD2;
				calculate_etl2(dWD);
				if (current > max_current) current = max_current;
				else if (current < 0) current = 0;
				send_currentset_command(current);
				cout << "s_dWD2=" << s_dWD2 << endl;
				sleep(50);
			}
			//焦点距離変化（下矢印ボタン）
			if (GetAsyncKeyState(VK_DOWN)) {
				s_dWD2 = s_dWD2 - 0.005;
				//s_dWD2 = s_dWD2 - 0.3;
				dWD = WD - s_WD + s_dWD2;
				calculate_etl2(dWD);
				if (current > max_current) current = max_current;
				else if (current < 0) current = 0;
				send_currentset_command(current);
				cout << "s_dWD2=" << s_dWD2 << endl;
				sleep(50);
			}

			//man1の自動キャリブレーション（1&Cボタン）
			if (GetAsyncKeyState(0x31) && GetAsyncKeyState(0x43)) {
				get_man_calib_pettern(objectPoints1, imagePoints1, num_x, num_y, num_z, dx, dy, dz, dWD, hDevice_RU);
				printf("koko5");
				//send_man_manual_command;
				sleep(1000);
				//recieve_man_command();
				man1_flag = point_number;
			}

			//man2の自動キャリブレーション（2&Cボタン）
			if (GetAsyncKeyState(0x32) && GetAsyncKeyState(0x43)) {
				get_man_calib_pettern(objectPoints2, imagePoints2, num_x, num_y, num_z, dx, dy, dz, dWD, hDevice_LU);
				//send_man2_manual_command;
				sleep(1000);
				//recieve_man2_command();
				man2_flag = point_number;
			}

			//man3の自動キャリブレーション (3&Cボタン)
			if (GetAsyncKeyState(0x33) && GetAsyncKeyState(0x43)) {
				get_man_calib_pettern(objectPoints3, imagePoints3, num_x, num_y, num_z, dx, dy, dz, dWD, hDevice_RD);
				//send_man_manual_command;
				sleep(1000);
				//recieve_man_command();
				man3_flag = point_number;
			}

			//man4の自動キャリブレーション（4&Cボタン）
			if (GetAsyncKeyState(0x34) && GetAsyncKeyState(0x43)) {
				get_man_calib_pettern(objectPoints4, imagePoints4, num_x, num_y, num_z, dx, dy, dz, dWD, hDevice_LD);
				//send_man2_manual_command;
				sleep(1000);
				//recieve_man2_command();
				man4_flag = point_number;
			}

			//man1の座標を取得後、pnp問題を解く
			if (man1_flag == point_number) {		//外部パラメータ求める方
			//f_k = (fObETOL + fTL) * 1000 / pixelpitch;
				f_k = (fObETOL + fTL) * 1000 / pixelpitch * magnification;
				K = (Mat_<double>(3, 3) << f_k, 0, cx, 0, f_k, cy, 0, 0, 1);

				solvePnP(objectPoints1, imagePoints1, K, distortion, rvec1, tvec1);	//外部パラメータ求める方

				cout << rvec1.at<double>(0, 0) << ", " << rvec1.at<double>(1, 0) << ", " << rvec1.at<double>(2, 0) << endl;
				cout << tvec1.at<double>(0, 0) << ", " << tvec1.at<double>(1, 0) << ", " << tvec1.at<double>(2, 0) << endl;
				Rodrigues(rvec1, rmat1);
				man1_flag = point_number + 2;
			}

			//man2の座標を取得後、pnp問題を解く　calib用
			if (man2_flag == point_number) {	//外部パラメータ求める方
				f_k = (fObETOL + fTL) * 1000 / pixelpitch * magnification;
				//f_k = (220) * 1000 / pixelpitch * magnification;
				//cout << fObETOL << "  " << fTL << endl;
				//f_k = (fObETOL + fTL) * 1000 / pixelpitch;
				K = (Mat_<double>(3, 3) << f_k, 0, cx, 0, f_k, cy, 0, 0, 1);

				solvePnP(objectPoints2, imagePoints2, K, distortion, rvec2, tvec2);	//外部パラメータ求める方

				cout << rvec2.at<double>(0, 0) << ", " << rvec2.at<double>(1, 0) << ", " << rvec2.at<double>(2, 0) << endl;
				cout << tvec2.at<double>(0, 0) << ", " << tvec2.at<double>(1, 0) << ", " << tvec2.at<double>(2, 0) << endl;

				Rodrigues(rvec2, rmat2);//「光軸方向にスキャン」でいるらしい（３次元用）
				printf("pnp2\n");
				man2_flag = point_number + 2;
			}

			//man3の座標を取得後、pnp問題を解く calib用
			if (man3_flag == point_number) {		//外部パラメータ求める方
			//f_k = (fObETOL + fTL) * 1000 / pixelpitch;
				f_k = (fObETOL + fTL) * 1000 / pixelpitch * magnification;
				K = (Mat_<double>(3, 3) << f_k, 0, cx, 0, f_k, cy, 0, 0, 1);

				solvePnP(objectPoints3, imagePoints3, K, distortion, rvec3, tvec3);	//外部パラメータ求める方

				cout << rvec3.at<double>(0, 0) << ", " << rvec3.at<double>(1, 0) << ", " << rvec3.at<double>(2, 0) << endl;
				cout << tvec3.at<double>(0, 0) << ", " << tvec3.at<double>(1, 0) << ", " << tvec3.at<double>(2, 0) << endl;
				Rodrigues(rvec3, rmat3);
				man3_flag = point_number + 2;

			}

			if (man4_flag == point_number) {	//外部パラメータ求める方
			//man4の座標を取得後、pnp問題を解く　calib用
				f_k = (fObETOL + fTL) * 1000 / pixelpitch * magnification;
				//f_k = (220) * 1000 / pixelpitch * magnification;
				//cout << fObETOL << "  " << fTL << endl;
				//f_k = (fObETOL + fTL) * 1000 / pixelpitch;
				K = (Mat_<double>(3, 3) << f_k, 0, cx, 0, f_k, cy, 0, 0, 1);

				solvePnP(objectPoints4, imagePoints4, K, distortion, rvec4, tvec4);	//外部パラメータ求める方

				cout << rvec4.at<double>(0, 0) << ", " << rvec4.at<double>(1, 0) << ", " << rvec4.at<double>(2, 0) << endl;
				cout << tvec4.at<double>(0, 0) << ", " << tvec4.at<double>(1, 0) << ", " << tvec4.at<double>(2, 0) << endl;

				Rodrigues(rvec4, rmat4);//「光軸方向にスキャン」でいるらしい（３次元用）
				printf("pnp4\n");
				man4_flag = point_number + 2;
			}

			//circle_img のマウス入力（グローバルの g_circleMouse に記録する。T＋クリックはクリック移動の目標設定）
			//以前はループ内のローカル変数を登録していたため、クリックを正しく受け取れていなかった
			setMouseCallback("circle_img", CircleImgMouseCallback, NULL);
			mouseParam mouse = g_circleMouse;
			g_circleMouse.event = -1;	//同じクリックを次のループで再判定しない

			//man1の先端位置の修正（１ボタン＆circle_imgの先端を左クリック）
			if (man1_flag >= point_number + 1 && GetAsyncKeyState(0x31) && mouse.event == cv::EVENT_LBUTTONDOWN) {
				man1_dx = mouse.x - man1_pos2D.at<double>(0, 0);
				man1_dy = mouse.y - man1_pos2D.at<double>(0, 1);
			}

			//man2の先端位置の修正（２ボタン＆circle_imgの先端を左クリック）
			if (man2_flag >= point_number + 1 && GetAsyncKeyState(0x32) && mouse.event == cv::EVENT_LBUTTONDOWN) {
				man2_dx = mouse.x - man2_pos2D.at<double>(0, 0);
				man2_dy = mouse.y - man2_pos2D.at<double>(0, 1);
			}

			//man3の先端位置の修正 (3ボタン&circle_imgの先端を左クリック)
			if (man3_flag >= point_number + 1 && GetAsyncKeyState(0x33) && mouse.event == cv::EVENT_LBUTTONDOWN) {
				man3_dx = mouse.x - man3_pos2D.at<double>(0, 0);
				man3_dy = mouse.y - man3_pos2D.at<double>(0, 1);
			}

			//man4の先端位置の修正　(4ボタン%circle_imgの先端を左クリック)
			if (man4_flag >= point_number + 1 && GetAsyncKeyState(0x34) && mouse.event == cv::EVENT_LBUTTONDOWN) {
				man4_dx = mouse.x - man4_pos2D.at<double>(0, 0);
				man4_dy = mouse.y - man4_pos2D.at<double>(0, 1);
			}

			//man4の３次元座標をカメラ座標系に変換
			if (man4_flag == point_number + 2) {
				//cout << rvec2 << endl;
				//cout << tvec2 << endl;
				//cout << K << endl;
				//cout << distortion << endl;

				projectPoints(man4_pos3D, rvec4, tvec4, K, distortion, man4_pos2D);
				//cout << "man2_3D" << man2_pos3D << endl;
				//cout << "man2_2D" << man2_pos2D << endl;
				//cout << "rvec2" << rvec2 << endl;
				//cout << "tvec2" << tvec2 << endl;
				//cout << "K2" << K << endl;
				//cout << "distortion2" << distortion << endl;
				//cout << man2_pos2D << endl;
				man4_cpos[0] = man4_pos2D.at<double>(0, 0) + man4_dx;
				man4_cpos[1] = man4_pos2D.at<double>(0, 1) + man4_dy;
				//man2_zc = rmat2.at<double>(0, 2) * man2_pos3D.at<double>(0, 0) + rmat2.at<double>(1, 2) * man2_pos3D.at<double>(0, 1) + rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);
				man4_zc = rmat4.at<double>(2, 2) * man4_pos3D.at<double>(0, 2) + tvec4.at<double>(2, 0);//ピペットをｘ－ｙ平面上で動かすとｚ座標が変わったしまったため、応急処置。もともとのコードは→//man2_zc = rmat2.at<double>(0, 2) * man2_pos3D.at<double>(0, 0) + rmat2.at<double>(1, 2) * man2_pos3D.at<double>(0, 1) + rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);
				//man2_cpos[2] = (man2_zc - man2_zc0 + (man2_s_dWD2 - s_dWD_0) * 1000) / pixelpitch;
				man4_cpos[2] = (man4_zc - man4_zc0 + (man4_s_dWD2 - s_dWD_0) * 1000) / pixelpitch * magnification;
				double man4_zc2 = man4_zc * 1000 / pixelpitch * magnification;
				//printf("x,y=(%f  ,%f\n", man2_cpos[0], man2_cpos[1]);
				//printf("cam2(zc,zc0,dWD,dWD0)=(%f,%f,%f,%f)\n", man2_zc, man2_zc0, man2_s_dWD2, s_dWD_0);
				//printf("cam(x2,y2,z2,z2_2,i)=(%f,%f,%f,%f,%d)\n", man2_cpos[0], man2_cpos[1], man2_cpos[2], man2_zc2, man2_flag);
			}

			//man1の３次元座標をカメラ座標系に変換
			if (man1_flag == point_number + 2) {
				projectPoints(man1_pos3D, rvec1, tvec1, K, distortion, man1_pos2D);
				//cout <<"man1_3D" << man1_pos3D << endl;
				//cout <<"man1_2D"<< man1_pos2D << endl;
				//cout <<"rvec1"<< rvec1 << endl;
				//cout <<"tvec"<< tvec1 << endl;
				//cout <<"K1"<< K << endl;
				//cout <<"distortion1"<< distortion << endl;

				//temp3_man1_pos2D[0] = temp2_man1_pos2D[0];//３ループ前のデータを保存
				//temp3_man1_pos2D[1] = temp2_man1_pos2D[1];//３ループ前のデータを保存

				//temp2_man1_pos2D[0] = temp1_man1_pos2D[0];//２ループ前のデータを保存
				//temp2_man1_pos2D[1] = temp1_man1_pos2D[1];//２ループ前のデータを保存

				//temp1_man1_pos2D[0] = man1_cpos[0];//１ループ前のデータを保存
				//temp1_man1_pos2D[1] = man1_cpos[1];//１ループ前のデータを保存

				//past_man1_cpos[0] = man1_cpos[0];//１ループ前のデータを保存
				//past_man1_cpos[1] = man1_cpos[1];//１ループ前のデータを保存
				//past_man1_cpos[2] = man1_cpos[2];//１ループ前のデータを保存

				man1_cpos[0] = man1_pos2D.at<double>(0, 0) + man1_dx;
				man1_cpos[1] = man1_pos2D.at<double>(0, 1) + man1_dy;
				man1_zc = rmat1.at<double>(0, 2) * man1_pos3D.at<double>(0, 0) + rmat1.at<double>(1, 2) * man1_pos3D.at<double>(0, 1) + rmat1.at<double>(2, 2) * man1_pos3D.at<double>(0, 2) + tvec1.at<double>(2, 0);
				//man1_cpos[2] = (man1_zc - man1_zc0 + (man1_s_dWD2 - s_dWD_0) * 1000) / pixelpitch;
				man1_cpos[2] = (man1_zc - man1_zc0 + (man1_s_dWD2 - s_dWD_0) * 1000) / pixelpitch * magnification;
				double man1_zc2 = man1_zc * 1000 / pixelpitch * magnification;
				//printf("x,y=%f  ,%f\n", man1_cpos[0], man1_cpos[1]);
				//printf("cam1(zc,zc0,dWD,dWD0)=(%f,%f,%f,%f)\n", man1_zc, man1_zc0, man1_s_dWD2, s_dWD_0);
				//printf("cam(x1,y1,z1,z1_2,i)=(%f,%f,%f,%f,%d)\n", man1_cpos[0], man1_cpos[1], man1_cpos[2], man1_zc2, man1_flag);
			}

			//man3の３次元座標をカメラ座標系に変換（通常視野でも man3 の先端位置を計算する。拡張視野側と同じ処理）
			if (man3_flag == point_number + 2) {
				projectPoints(man3_pos3D, rvec3, tvec3, K, distortion, man3_pos2D);
				man3_cpos[0] = man3_pos2D.at<double>(0, 0) + man3_dx;
				man3_cpos[1] = man3_pos2D.at<double>(0, 1) + man3_dy;
				man3_zc = rmat3.at<double>(2, 2) * man3_pos3D.at<double>(0, 2) + tvec3.at<double>(2, 0);
				man3_cpos[2] = (man3_zc - man3_zc0 + (man3_s_dWD2 - s_dWD_0) * 1000) / pixelpitch * magnification;
			}

			//man2の３次元座標をカメラ座標系に変換
			if (man2_flag == point_number + 2) {
				//cout << rvec2 << endl;
				//cout << tvec2 << endl;
				//cout << K << endl;
				//cout << distortion << endl;

				projectPoints(man2_pos3D, rvec2, tvec2, K, distortion, man2_pos2D);
				//cout << "man2_3D" << man2_pos3D << endl;
				//cout << "man2_2D" << man2_pos2D << endl;
				//cout << "rvec2" << rvec2 << endl;
				//cout << "tvec2" << tvec2 << endl;
				//cout << "K2" << K << endl;
				//cout << "distortion2" << distortion << endl;
				//cout << man2_pos2D << endl;
				man2_cpos[0] = man2_pos2D.at<double>(0, 0) + man2_dx;
				man2_cpos[1] = man2_pos2D.at<double>(0, 1) + man2_dy;
				//man2_zc = rmat2.at<double>(0, 2) * man2_pos3D.at<double>(0, 0) + rmat2.at<double>(1, 2) * man2_pos3D.at<double>(0, 1) + rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);
				man2_zc = rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);//ピペットをｘ－ｙ平面上で動かすとｚ座標が変わったしまったため、応急処置。もともとのコードは→//man2_zc = rmat2.at<double>(0, 2) * man2_pos3D.at<double>(0, 0) + rmat2.at<double>(1, 2) * man2_pos3D.at<double>(0, 1) + rmat2.at<double>(2, 2) * man2_pos3D.at<double>(0, 2) + tvec2.at<double>(2, 0);
				//man2_cpos[2] = (man2_zc - man2_zc0 + (man2_s_dWD2 - s_dWD_0) * 1000) / pixelpitch;
				man2_cpos[2] = (man2_zc - man2_zc0 + (man2_s_dWD2 - s_dWD_0) * 1000) / pixelpitch * magnification;
				double man2_zc2 = man2_zc * 1000 / pixelpitch * magnification;
				//printf("x,y=(%f  ,%f\n", man2_cpos[0], man2_cpos[1]);
				//printf("cam2(zc,zc0,dWD,dWD0)=(%f,%f,%f,%f)\n", man2_zc, man2_zc0, man2_s_dWD2, s_dWD_0);
				//printf("cam(x2,y2,z2,z2_2,i)=(%f,%f,%f,%f,%d)\n", man2_cpos[0], man2_cpos[1], man2_cpos[2], man2_zc2, man2_flag);
			}

			//座標変換と逆変換の誤差を埋める(ホールディング2　2&ctrl)
			if (GetAsyncKeyState(0x32) && GetAsyncKeyState(0xA2))
			{
				Getposition(hDevice_LD, man4_xw, man4_yw, man4_zw);
				man4_pos3D.at<double>(0, 0) = man4_xw;
				man4_pos3D.at<double>(0, 1) = -man4_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
				man4_pos3D.at<double>(0, 2) = man4_zw;//三次元時
				printf("cam(x2,y2,z2,z2_2,i)=(%f,%f,%f,%f,%d)\n", man4_cpos[0], man4_cpos[1], man4_cpos[2], man4_zc, man4_flag);
				camera_to_world2(man4_cpos, temp_man4_pos3D);
				CtoW_man4_dx = man4_pos3D.at<double>(0, 0) - temp_man4_pos3D[0];
				CtoW_man4_dy = man4_pos3D.at<double>(0, 1) - temp_man4_pos3D[1];
				CtoW_man4_dz = man4_pos3D.at<double>(0, 2) - temp_man4_pos3D[2];
				std::cout << "man4_pos3Dx=" << man4_pos3D.at<double>(0, 0) << std::endl;
				std::cout << "man4_pos3Dy=" << man4_pos3D.at<double>(0, 1) << std::endl;
				std::cout << "man4_pos3Dz=" << man4_pos3D.at<double>(0, 2) << std::endl;
				std::cout << "temp_man4_pos3Dx=" << temp_man4_pos3D[0] << std::endl;
				std::cout << "temp_man4_pos3Dy=" << temp_man4_pos3D[1] << std::endl;
				std::cout << "temp_man4_pos3Dz=" << temp_man4_pos3D[2] << std::endl;
				std::cout << "CtoW_man4_dx=" << CtoW_man4_dx << std::endl;
				std::cout << "CtoW_man4_dy=" << CtoW_man4_dy << std::endl;
				std::cout << "CtoW_man4_dz=" << CtoW_man4_dz << std::endl;
				sleep(500);
			}

			//座標変換と逆変換の誤差を埋める(ホールディング1　1&ctrl)
			if (GetAsyncKeyState(0x31) && GetAsyncKeyState(0xA2))
			{
				Getposition(hDevice_LU, man2_xw, man2_yw, man2_zw);
				man2_pos3D.at<double>(0, 0) = man2_xw;
				man2_pos3D.at<double>(0, 1) = -man2_yw;//カメラ座標系にy軸の向きを合わせるためマイナスをつける
				man2_pos3D.at<double>(0, 2) = man2_zw;//三次元時
				printf("cam(x2,y2,z2,z2_2,i)=(%f,%f,%f,%f,%d)\n", man2_cpos[0], man2_cpos[1], man2_cpos[2], man2_zc, man2_flag);
				camera_to_world1(man2_cpos, temp_man2_pos3D);
				CtoW_man2_dx = man2_pos3D.at<double>(0, 0) - temp_man2_pos3D[0];
				CtoW_man2_dy = man2_pos3D.at<double>(0, 1) - temp_man2_pos3D[1];
				CtoW_man2_dz = man2_pos3D.at<double>(0, 2) - temp_man2_pos3D[2];
				std::cout << "man2_pos2Dx=" << man2_pos2D.at<double>(0, 0) << std::endl;
				std::cout << "man2_pos2Dy=" << man2_pos2D.at<double>(0, 1) << std::endl;
				std::cout << "man2_pos2Dz=" << man2_pos2D.at<double>(0, 2) << std::endl;
				std::cout << "man2_pos3Dx=" << man2_pos3D.at<double>(0, 0) << std::endl;
				std::cout << "man2_pos3Dy=" << man2_pos3D.at<double>(0, 1) << std::endl;
				std::cout << "man2_pos3Dz=" << man2_pos3D.at<double>(0, 2) << std::endl;
				std::cout << "temp_man2_pos3Dx=" << temp_man2_pos3D[0] << std::endl;
				std::cout << "temp_man2_pos3Dy=" << temp_man2_pos3D[1] << std::endl;
				std::cout << "temp_man2_pos3Dz=" << temp_man2_pos3D[2] << std::endl;
				std::cout << "CtoW_man2_dx=" << CtoW_man2_dx << std::endl;
				std::cout << "CtoW_man2_dy=" << CtoW_man2_dy << std::endl;
				std::cout << "CtoW_man2_dz=" << CtoW_man2_dz << std::endl;
				sleep(500);
			}

			////man1の描写
			if (man1_flag >= point_number) {
				cv::circle(circle_img, cv::Point((int)man1_cpos[0], (int)man1_cpos[1]), 2, cv::Scalar(0, 200, 0), 3);
				//cv::circle(circle_img, cv::Point((int)man1_pos2D.at<double>(0, 0), (int)man1_pos2D.at<double>(0, 1)), 2, cv::Scalar(200, 0, 0), 3);
			}

			////man2の描写
			if (man2_flag >= point_number) {
				cv::circle(circle_img, cv::Point((int)man2_cpos[0], (int)man2_cpos[1]), 2, cv::Scalar(0, 200, 0), 3);
				//cv::circle(circle_img, cv::Point((int)man2_pos2D.at<double>(0, 0), (int)man2_pos2D.at<double>(0, 1)), 2, cv::Scalar(200, 0, 0), 3);
			}

			////man3の描写
			if (man3_flag >= point_number) {
				cv::circle(circle_img, cv::Point((int)man3_cpos[0], (int)man3_cpos[1]), 2, cv::Scalar(0, 200, 0), 3);
				//cv::circle(circle_img, cv::Point((int)man1_pos2D.at<double>(0, 0), (int)man1_pos2D.at<double>(0, 1)), 2, cv::Scalar(200, 0, 0), 3);
			}

			////man4の描写
			if (man4_flag >= point_number) {
				cv::circle(circle_img, cv::Point((int)man4_cpos[0], (int)man4_cpos[1]), 2, cv::Scalar(0, 200, 0), 3);
				//cv::circle(circle_img, cv::Point((int)man4_pos2D.at<double>(0, 0), (int)man4_pos2D.at<double>(0, 1)), 2, cv::Scalar(200, 0, 0), 3);
			}

			//画像中心の描写
			if (set_flag == 3) {
				cv::circle(circle_img, cv::Point(IMG_WIDTH / 2, IMG_HEIGHT / 2), 2, cv::Scalar(0, 200, 0), 3);
			}

			//高さと焦点面の取得 //ctrl3~
			//左右のピペットの高さを合わせる
			if (set_flag == 1 && GetAsyncKeyState(0x33) && GetAsyncKeyState(0xA2)) {
				McsdSetCounter(hDevice_RU, 2, 0, 50000);
				McsdSetCounter(hDevice_LU, 2, 0, 50000);
				McsdSetCounter(hDevice_RD, 2, 0, 50000);
				McsdSetCounter(hDevice_LD, 2, 0, 50000);
				sleep(1000);
				set_flag = 2;
				printf("set1");
			}
			//左上に右下を合わせる
			if (set_flag == 2 && GetAsyncKeyState(0x34) && GetAsyncKeyState(0xA2)) {
				Getposition(hDevice_RD, set_man3_xw, set_man3_yw, set_man3_zw);
				Getposition(hDevice_LD, set_man4_xw, set_man4_yw, set_man4_zw);
				sleep(50);
				std::vector<double> move_pos{ set_man4_xw,set_man4_yw,set_man3_zw };
				Movepos(hDevice_LD, s, move_pos);
				set_flag = 3;
				printf("set2");
			}

			//下限設定
			if (GetAsyncKeyState(0x35) && GetAsyncKeyState(0xA2)) {
				Getposition(hDevice_RD, set_man3_xw, set_man3_yw, set_man3_zw);
				cout << "s_dWD2_botom=" << s_dWD2 << endl;
				cout << "bottom_man_z=" << set_man3_zw << endl;
				sleep(50);
				set_flag = 4;
				printf("set3");
			}

			//作業高さ設定1
			if (GetAsyncKeyState(0x36) && GetAsyncKeyState(0xA2)) {
				Getposition(hDevice_RU, set_man1_xw, set_man1_yw, set_man1_zw);
				std::vector<double> move_pos{ set_man1_xw,set_man1_yw,-2500 + 300 };
				Movepos(hDevice_RU, s, move_pos);
				sleep(1000);
				set_flag = 5;
				printf("set4");
			}

			//作業高さ設定2
			if (GetAsyncKeyState(0x37) && GetAsyncKeyState(0xA2)) {
				cout << "s_dWD2_work=" << s_dWD2 << endl;
				sleep(50);
				set_flag = 0;
				printf("set5");
			}

			////終了処理 (キャリブレーションができているとき、ピペットを規定の位置に移動) ctrl + 0
			//if (GetAsyncKeyState(0x30) && GetAsyncKeyState(0xA2)) {
			//	move_mode = 0;
			//	use = 1;
			//	auto_flag = 1;
			//	sleep(10);
			//	std::vector<double> move_pos1{ 2800,-2200,-2000 }; //右上:2700, -2300, 2500
			//	Movepos(hDevice_RU, s, move_pos1);
			//	sleep(100);
			//	std::vector<double> move_pos2{ 2200,-2200,-2000 }; //左上:2400, -2400, 2500
			//	Movepos(hDevice_LU, s, move_pos2);
			//	sleep(100);
			//	std::vector<double> move_pos3{ 2800,-2800,-2000 }; //右下:2600, -2600, 2500
			//	Movepos(hDevice_RD, s, move_pos3);
			//	sleep(100);
			//	std::vector<double> move_pos4{ 2200,-2800,-2000 }; //左下:2400, -2600, 2500
			//	Movepos(hDevice_LD, s, move_pos4);
			//	sleep(100);
			//	printf("end");
			//}

			//搬送準備 (キャリブレーションができているとき、ピペットを規定の位置に移動) ctrl + 0
			if (GetAsyncKeyState(0x30) && GetAsyncKeyState(0xA2)) {
				move_mode = 0;
				use = 1;
				//auto_flag = 1;
				sleep(10);
				std::vector<double> move_pos1{ 2800,-2200,-2200 }; //右上:2700, -2300, 2500
				Movepos(hDevice_RU, s, move_pos1);
				sleep(100);
				std::vector<double> move_pos2{ 2200,-2200,-2200 }; //左上:2400, -2400, 2500
				Movepos(hDevice_LU, s, move_pos2);
				sleep(100);
				std::vector<double> move_pos3{ 2800,-2800,-2200 }; //右下:2600, -2600, 2500
				Movepos(hDevice_RD, s, move_pos3);
				sleep(100);
				std::vector<double> move_pos4{ 2200,-2800,-2200 }; //左下:2400, -2600, 2500
				Movepos(hDevice_LD, s, move_pos4);
				sleep(100);
				printf("end");
			}

			//高さ合わせ ctrl + 9
			if (GetAsyncKeyState(0x39) && GetAsyncKeyState(0xA2)) {
				move_mode = 0;
				use = 1;
				//auto_flag = 1;
				sleep(10);
				std::vector<double> move_pos1{ 2800,-2200,-2500 }; //右上:2700, -2300, 2500
				Movepos(hDevice_RU, s, move_pos1);
				sleep(100);
				std::vector<double> move_pos2{ 2200,-2200,-2500 }; //左上:2400, -2400, 2500
				Movepos(hDevice_LU, s, move_pos2);
				sleep(100);
				std::vector<double> move_pos3{ 2800,-2800,-2500 }; //右下:2600, -2600, 2500
				Movepos(hDevice_RD, s, move_pos3);
				sleep(100);
				std::vector<double> move_pos4{ 2200,-2800,-2500 }; //左下:2400, -2600, 2500
				Movepos(hDevice_LD, s, move_pos4);
				sleep(2000);
				printf("end");
				move_mode = 2;
				use = 2;
			}

			// 大島追加
			// ↓↓↓ ここからROI描画コードを追加 ↓↓↓
			// ★ AIが使用中のROIを黄色の四角で描画
			if (g_ai_motor_engaged.load() || g_ai_capture_request.load())
			{
				cv::rectangle(circle_img, ai_roi, cv::Scalar(0, 255, 255), 2); // BGR: 黄色
			}

			cv::Scalar ai_color(0, 255, 0);
			std::string ai_text = "";
			/*if (g_ai_motor_engaged.load()) {
				double current_progress = g_ai_progress.load();
				char ai_buffer[100];
				sprintf(ai_buffer, "[AI ACTIVE] %.1f%% (%.1f FPS)", current_progress * 100.0);
				ai_text = std::string(ai_buffer);
				ai_color = g_ai_is_stop.load() ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 165, 255);
			}
			else if (g_ai_capture_request.load()) {
				ai_text = "[AI STARTING...]";
				ai_color = cv::Scalar(0, 255, 255);
			}
			else {
				ai_text = "[AI STANDBY]";
			}*/
			if (g_ai_motor_engaged.load()) {
				// double型の変数をint型(クラス番号)として読み込む
				int current_class = static_cast<int>(g_ai_progress.load());

				// クラス番号に応じた状態名を設定
				const char* class_name = "UNKNOWN";
				if (current_class == 0) class_name = "0:WAIT"; // 待機
				else if (current_class == 1) class_name = "1:BUSY"; // 動作中
				else if (current_class == 2) class_name = "2:DONE"; // 完了

				char ai_buffer[100];
				// 文字列としてフォーマット (%.1f%% を %s に変更)
				// [AI ACTIVE] 状態名 (FPS) という形式で表示
				sprintf(ai_buffer, "[AI ACTIVE] %s (%.1f FPS)", class_name, g_ai_fps.load());

				ai_text = std::string(ai_buffer);

				// 停止信号が出たら赤色、それ以外はオレンジ色
				ai_color = g_ai_is_stop.load() ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 165, 255);
			}
			else if (g_ai_injection_done.load()) {
				ai_text = "Elimination DONE!";
				ai_color = cv::Scalar(0, 255, 0); // 緑色で成功を表示
			}
			// circle_img に描画
			cv::putText(circle_img, ai_text, cv::Point(20, 160),
				cv::FONT_HERSHEY_SIMPLEX, 1.2, ai_color, 3);
			// 実効fps表示
			std::ostringstream ss;
			ss << std::fixed << std::setprecision(1)
				<< "Real FPS: " << display_fps  // 計算済みのFPSを表示
				<< " / AI FPS: " << g_ai_fps.load();

			cv::putText(circle_img,
				ss.str(),
				cv::Point(10, 30),
				cv::FONT_HERSHEY_SIMPLEX,
				1.0,
				cv::Scalar(0, 255, 255),
				2);

			// ==========================================
			// ▼▼▼ 非同期・ROI録画ロジック ▼▼▼
			// ==========================================
			// 録画条件: AI動作中 or 開始要求中 or 完了表示中
			// かつ、ROIの面積が0より大きい（正しく設定されている）こと
			bool should_record = (g_ai_motor_engaged.load() || g_ai_capture_request.load() )//|| g_ai_injection_done.load()
				&& (ai_roi.area() > 0);
			if (should_record) {
				// 安全なROI（画像範囲外へのはみ出し防止）を作成
				cv::Rect safe_roi = ai_roi & cv::Rect(0, 0, circle_img.cols, circle_img.rows);
				// --- 1. ファイルオープンの処理（まだ開いていなければ） ---
				if (!g_recorder.isOpened()) {
					time_t t = time(nullptr);
					struct tm tm_now;
					localtime_s(&tm_now, &t);
					char filename[64];
					strftime(filename, sizeof(filename), "ai_roi_%Y%m%d_%H%M%S.mp4", &tm_now); // .avi -> .mp4
					// ★ ROIのサイズでVideoWriterを開く（これが軽量化の鍵）
					g_recorder.open(filename, cv::VideoWriter::fourcc('H', '2', '6', '4'), 15.0, safe_roi.size(), true);
					if (g_recorder.isOpened()) {
						std::cout << "[REC] Started Async (Size: " << safe_roi.width << "x" << safe_roi.height << ")" << std::endl;
						g_is_recording_active = true; // ★フラグをON
					}
				}
				// --- 2. 画像をキューに追加する処理 ---
				if (g_recorder.isOpened()) {
					// ★重要: clone()でメモリをコピーして切り離す
					cv::Mat roi_clone = circle_img(safe_roi).clone();
					{
						std::lock_guard<std::mutex> lock(g_queue_mutex);
						// メモリあふれ防止（念のため100フレーム以上溜まったら追加しない）
						if (g_frame_queue.size() < 100) {
							g_frame_queue.push(roi_clone);
						}
					}
					// 録画スレッドを起こす
					g_queue_cv.notify_one();
				}
			}
			else {
				if (g_is_recording_active) {
					// ★直接 release() せず、ワーカーに「終了合図（空画像）」を送る
					{
						std::lock_guard<std::mutex> lock(g_queue_mutex);
						g_frame_queue.push(cv::Mat()); // 空っぽの画像を入れる
					}
					g_queue_cv.notify_one();
					g_is_recording_active = false; // ★フラグをOFF
					std::cout << "[REC] Main: Stop signal sent." << std::endl;
				}
			}
			// ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲
			//追加ここまで

			//==================== クリック移動：目標の表示と Enter / X の判定 ====================
			{
				static bool click_prev_enter = false;
				const bool click_enter_now = (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;

				//X キー：移動中なら停止、そうでなければ目標をクリア
				if (GetAsyncKeyState(0x58) & 0x8000) {
					if (g_click_move_busy) {
						g_click_move_abort = true;
					}
					else if (g_click_target_set) {
						g_click_target_set = false;
						printf("[クリック移動] 目標をクリアしました\n");
					}
				}

				const int click_mode = g_click_move_busy ? -1 : move_mode;	//移動中は move_mode=0 になっているので表示用に使わない
				if (g_click_target_set) {
					cv::Point tp((int)g_click_target_px[0], (int)g_click_target_px[1]);
					cv::drawMarker(circle_img, tp, cv::Scalar(0, 0, 0), cv::MARKER_CROSS, 31, 5);
					cv::drawMarker(circle_img, tp, cv::Scalar(255, 255, 255), cv::MARKER_CROSS, 27, 2);

					std::string label = g_click_move_busy ? "MOVING" : "TARGET";
					if (click_mode == 1 || click_mode == 2) {
						const bool m1 = (click_mode == 2);
						double* cpos = m1 ? man1_cpos : man3_cpos;
						Mat& p3 = m1 ? man1_pos3D : man3_pos3D;
						cv::line(circle_img, cv::Point((int)cpos[0], (int)cpos[1]), tp, cv::Scalar(255, 255, 255), 1);
						double xt, yt;
						if ((m1 ? man1_flag : man3_flag) == point_number + 2 &&
							click_move::pixelToWorldFixedZ(g_click_target_px[0], g_click_target_px[1], p3.at<double>(0, 2),
								m1 ? rvec1 : rvec3, m1 ? tvec1 : tvec3, K, m1 ? man1_dx : man3_dx, m1 ? man1_dy : man3_dy, xt, yt)) {
							const double ddx = xt - p3.at<double>(0, 0);
							const double ddy = yt - (-p3.at<double>(0, 1));
							char buf[64];
							sprintf_s(buf, " %s %.0f um", m1 ? "man1" : "man3", sqrt(ddx * ddx + ddy * ddy));
							label += buf;
						}
					}
					cv::putText(circle_img, label, tp + cv::Point(18, -12), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 4);
					cv::putText(circle_img, label, tp + cv::Point(18, -12), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1);
				}

				//Enter キー（押した瞬間だけ）：移動開始
				if (click_enter_now && !click_prev_enter && g_click_target_set && !g_click_move_busy) {
					const int mode = move_mode;
					if (mode != 1 && mode != 2) {
						printf("[クリック移動] 操作中のインジェクションピペットがありません (move_mode=%d)\n", mode);
					}
					else if ((mode == 2 ? man1_flag : man3_flag) != point_number + 2) {
						printf("[クリック移動] %s のキャリブレーションが未完了のため移動できません\n", mode == 2 ? "man1" : "man3");
					}
					else {
						g_click_move_abort = false;
						g_click_move_busy = true;	//二重起動防止のため、スレッド起動前に立てる
						std::thread(clickMoveExecute, g_click_target_px[0], g_click_target_px[1], mode).detach();
					}
				}
				click_prev_enter = click_enter_now;
			}

			imshow("circle_img", circle_img);
			waitKey(1);
		}
		if (GetAsyncKeyState(VK_ESCAPE)) {
			multi_thrd_ctrl_looptime.close();
			end_flag = 1;
			break;
		}
		QueryPerformanceCounter(&end);
		loop_time = ((double)end.QuadPart - (double)start.QuadPart) / (double)qpf.QuadPart * 1000;

		//大島追加
		// ループ時間の計算
		loop_time = ((double)end.QuadPart - (double)start.QuadPart) / (double)qpf.QuadPart * 1000;
		// 次のフレームで表示するためのFPSを計算して保存
		if (loop_time > 0) {
			display_fps = 1000.0 / loop_time;
		}
		//cout << "loop_time = " << loop_time << endl;
		if (end_flag == 1) break;

		////データ保存 20250106
		//if (file_flag == true) {
		//	//counter_time += loop_time / 1000;
		//	data_file1 << t << ","
		//		<< DA_Data[0] - 32244 << "," << -th[0] << "," << dwCounter[0] << "," << DA_Data[1] - 32244 << "," << -th[1] << "," << dwCounter[1] << "," << DA_Data[2] - 32244 << "," << -th[2] << "," << dwCounter[2] << "," << DA_Data[3] - 32244 << "," << -th[3] <<"," << dwCounter[3] << endl;
		//	//printf("記録中");
		//}
	}
	sleep(1000);

	for (int thread_No = 0; thread_No < thread_maxNo; thread_No++) {
		while (1) {
			GetExitCodeThread(hTh[thread_No], &dwExCode[thread_No]);
			if (dwExCode[thread_No] == STILL_ACTIVE) {
				printf("スレッド%d稼働中\n", thread_No);
				sleep(1000);
			}
			else {
				break;
			}
		}
	}//すべてのスレッドが終了するまで待機

	destroyAllWindows();
	printf("親を終了します\n");
	for (int thread_No = 0; thread_No < thread_maxNo; thread_No++)
		CloseHandle(hTh[thread_No]);
	/*
	//グラフの書き込み
	FILE* fp;
	char filename[20];
	char RECfilename[20];
	char RECimagefilename[30];
	char RECfilename2[20];
	char RECimagefilename2[30];
	int I;
	Mat input_image = imread("offset.png", IMREAD_ANYCOLOR);
	if (REC_switch == 1) {

		for (int npointed_count = 0; npointed_count < pointed_count; npointed_count++) {
			sprintf(filename, "ADOutput%d.csv", npointed_count);
			sprintf(RECfilename, "_REC%d", npointed_count);
			sprintf(RECfilename2, "_REC_cont%d", npointed_count);
			//ディレクトリ作成
			_mkdir(RECfilename);
			_mkdir(RECfilename2);
			fp = fopen(filename, "w");
			if (fp == NULL) exit(1);

			fprintf(fp, "count,Time[ms],xangle,yangle\n");

			int npointed_count2 = 0;
			for (;;) {

				unsigned char* pimage2 = ppointedimage[nstoreimage * npointed_count + npointed_count2];
				unsigned char* pimage3 = contrast_pimage;
				for (int image_col = 0; image_col < IMG_HEIGHT; image_col++) {
					for (int image_row = 0; image_row < IMG_WIDTH; image_row++) {
						//*pimage3=*pimage2;
						I = (int)*pimage2 - (int)input_image.data[image_col * IMG_WIDTH + image_row];
						if (I < 0) I = 0;
						else if (I > 255)I = 255;
						*pimage3 = (uchar)I;
						pimage2++;
						pimage3++;
					}
				}

				fprintf(fp, "%d,%f,%f,%f\n", npointed_count2, Time3[nstoreimage * npointed_count + npointed_count2], *ppointedangle[nstoreimage * npointed_count + npointed_count2], *(ppointedangle[nstoreimage * npointed_count + npointed_count2] + 1));
				sprintf(RECimagefilename, "_REC%d/source%d.png", npointed_count, npointed_count2);
				sprintf(RECimagefilename2, "_REC_cont%d/source%d.png", npointed_count, npointed_count2);
				Mat save_frame(IMG_HEIGHT, IMG_WIDTH, CV_8UC1, ppointedimage[nstoreimage * npointed_count + npointed_count2]);
				imwrite(RECimagefilename, save_frame);
				contrast(contrast_pimage);
				Mat save_frame_contrast(IMG_HEIGHT, IMG_WIDTH, CV_8UC1, contrast_pimage);
				imwrite(RECimagefilename2, save_frame_contrast);
				npointed_count2++;
				if (nstoreimage * npointed_count + npointed_count2 >= nstorepointedimage - 1) break;

			}
		}

		sprintf(filename, "ADOutput.csv");
		sprintf(RECfilename, "_REC");
		sprintf(RECfilename2, "_REC_cont");
		//ディレクトリ作成
		_mkdir(RECfilename);
		_mkdir(RECfilename2);
		fp = fopen(filename, "w");
		if (fp == NULL) exit(1);

		fprintf(fp, "count,Time[ms],xangle,yangle\n");

		int npointed_count2 = 0;
		for (;;) {

			unsigned char* pimage2 = pimage[npointed_count2];
			unsigned char* pimage3 = contrast_pimage;
			for (int image_col = 0; image_col < IMG_HEIGHT; image_col++) {
				for (int image_row = 0; image_row < IMG_WIDTH; image_row++) {
					//*pimage3=*pimage2;
					I = (int)*pimage2 - (int)input_image.data[image_col * IMG_WIDTH + image_row];
					if (I < 0) I = 0;
					else if (I > 255)I = 255;
					*pimage3 = (uchar)I;
					pimage2++;
					pimage3++;
				}
			}

			fprintf(fp, "%d,%f,%f,%f\n", npointed_count2, Time2[npointed_count2], *pangle[npointed_count2], *(pangle[npointed_count2] + 1));
			sprintf(RECimagefilename, "_REC/source%d.png", npointed_count2);
			sprintf(RECimagefilename2, "_REC_cont/source%d.png", npointed_count2);
			Mat save_frame(IMG_HEIGHT, IMG_WIDTH, CV_8UC1, pimage[npointed_count2]);
			imwrite(RECimagefilename, save_frame);
			contrast(contrast_pimage);
			Mat save_frame_contrast(IMG_HEIGHT, IMG_WIDTH, CV_8UC1, contrast_pimage);
			imwrite(RECimagefilename2, save_frame_contrast);
			npointed_count2++;
			if (npointed_count2 >= nstoreimage - 1) break;

		}
		free(contrast_pimage);
	}
	*/

	//グラフの書き込み
	FILE* fp2;
	char filename2[20];


	sprintf(filename2, "AT1_FPS.csv");
	fp2 = fopen(filename2, "w");
	if (fp2 == NULL) exit(1);
	fprintf(fp2, "count,Mag,expand_viewpoint_num,Time[ms],FPS\n");

	printf("thread1=%d, %d\n", s_count1, s_count2);
	for (int s_count1_2 = 0; s_count1_2 < s_count1; s_count1_2++) {
		for (int s_count2_2 = 0; s_count2_2 < sc_count2[s_count1_2]; s_count2_2++) {
			fprintf(fp2, "%d,%f, %d,%f,%f\n", s_count1_2, s_MAG[s_count1_2][s_count2_2], s_expand_viewpoint_num[s_count1_2][s_count2_2], s_time_frame[s_count1_2][s_count2_2], s_fps[s_count1_2][s_count2_2]);
		}
	}


	//グラフの書き込み
	FILE* fp3;
	char filename3[20];


	sprintf(filename3, "AT2_FPS.csv");
	fp3 = fopen(filename3, "w");
	if (fp3 == NULL) exit(1);
	fprintf(fp3, "count,pointed_count,Time[ms],FPS\n");

	printf("thread2=%d, %d\n", s_t2count1, s_t2count2);
	for (int s_count1_2 = 0; s_count1_2 < s_t2count1; s_count1_2++) {
		for (int s_count2_2 = 0; s_count2_2 < sc_t2count2[s_count1_2]; s_count2_2++) {
			fprintf(fp3, "%d,%d,%f,%f\n", s_count1_2, s_pointed_count[s_count1_2][s_count2_2], s_t2time_frame[s_count1_2][s_count2_2], s_t2fps[s_count1_2][s_count2_2]);
		}
	}
}

double FocalLengthTransform(double f1, double d1) {
	double f2, sin_a, sin_b, sin_c, tan_a, tan_b, tan_c;

	tan_a = D / 2.0 / f1;

	//sin_a = f1 * 2.0 / D;
	sin_a = tan_a / sqrt(1 + tan_a * tan_a);
	sin_b = sin_a / ng;
	sin_c = sin_a / nl;

	//tan_a = sin_a / sqrt(1 - sin_a * sin_a);
	tan_b = sin_b / sqrt(1 - sin_b * sin_b);
	tan_c = sin_c / sqrt(1 - sin_c * sin_c);

	d3 = (D / 2.0 - d1 * tan_a - d2 * tan_b) / tan_c;

	f2 = d1 + d2 + d3;

	return f2;
}

int check_negative(int a, int flag_a) {
	if (flag_a == 1) {
		a *= -1;
	}
	return a;
}

//string->wstring
wstring multi_to_wide_winapi(string const& src)
{
	auto const dest_size = ::MultiByteToWideChar(CP_ACP, 0U, src.data(), -1, nullptr, 0U);
	vector<wchar_t> dest(dest_size, L'\0');
	if (::MultiByteToWideChar(CP_ACP, 0U, src.data(), -1, dest.data(), dest.size()) == 0) {
		throw std::system_error{ static_cast<int>(::GetLastError()), system_category() };
	}
	dest.resize(char_traits<wchar_t>::length(dest.data()));
	dest.shrink_to_fit();
	return wstring(dest.begin(), dest.end());
}



int main() {

	open_ETLCOMPort();

	// カメラデバイスのハンドラを読み出す 
	printf("Opening first camera...\n");
	PUCRESULT result;
	/*result = PUC_Initialize();
	if (PUC_CHK_FAILED(result))
	{
		return 0;
	}
	PUC_DETECT_INFO detectInfo;
	result = PUC_DetectDevice(&detectInfo);
	if (PUC_CHK_FAILED(result))
	{
		return 0;
	}
	if (detectInfo.nDeviceCount == 0)
	{
		return 0;
	}*/
	//photron_camera open
	int deviceID = 0;             // 0 = open default camera
	int apiID = cv::CAP_ANY;      // 0 = autodetect default API
	// open selected camera using selected API
	//cout << "before_open" << endl;
	cap.open(deviceID, apiID);
	//cout << "after_open" << endl;
	Sleep(1000);
	//cout << "result" << result << endl;
	//cout << m_lastErrorName << endl;
	//cap.open(3, 1);
	// check if we succeeded
	if (!cap.isOpened()) {
		cerr << "ERROR! Unable to open camera\n";
		return -1;
	}

	PUC_HANDLE mHandle = cap.getPUCLibWrapper()->getPUCHandle();
	//cap.getPUCLibWrapper()->pause();
	cout << "\nbreak_point" << endl;
	//PUCRESULT result0 = cap.getPUCLibWrapper()->setFramerateShutter((UINT32)nFramerate, (UINT32)nShutterspeed); // 1000, 2000
	//cap.getPUCLibWrapper()->resume();
	PUCRESULT result1 = cap.getPUCLibWrapper()->getFramerateShutter(&nFramerate, &nShutterspeed);

	//result = PUC_GetMaxResolution(mHandle,max_width,max_height);
	//std::cout << "max_height=" << max_height;
	//result = PUC_SetResolution(mHandle, 1246, 1080);
	//if (PUC_CHK_FAILED(result))
	//{
	//	printf("failed");
	//	return 0;
	//}
	//printf("resolution setting");
	printf("framerateshutter setting returns %ld , %ld", nFramerate, nShutterspeed);

	/*----Ad_初期化----*/

	// デバイスのオープン
	sprintf(lpszName, "FBIAD%d", 1);
	hDeviceHandle_ad = AdOpen((LPCTSTR)lpszName);
	if (hDeviceHandle_ad == INVALID_HANDLE_VALUE) {
		printf("ADデバイスのオープンに失敗しました.\n");
		return -1;
	}
	/*----Ad_初期化_end----*/

	//Da open//
	//□デバイスのオープン DA--------------------------------------------------------
	sprintf(lpszName, "FBIDA%d", 1);
	ghDeviceHandle_da1 = DaOpen((LPCTSTR)lpszName);
	//ghDeviceHandle_da = DaOpen("FBIDA1");
	if (ghDeviceHandle_da1 == INVALID_HANDLE_VALUE) {
		printf("DAデバイス1のオープンに失敗しました.\n");
		//sleep(10000);
		return 0;
	}

	//Da open//
	//□デバイスのオープン DA--------------------------------------------------------
	sprintf(lpszName, "FBIDA%d", 2);
	ghDeviceHandle_da2 = DaOpen((LPCTSTR)lpszName);
	//ghDeviceHandle_da = DaOpen("FBIDA1");
	if (ghDeviceHandle_da2 == INVALID_HANDLE_VALUE) {
		printf("DAデバイス2のオープンに失敗しました.\n");
		//sleep(10000);
		return 0;
	}


	//DAボードの設定---------------------------------------------------------------
	ULONG ulCh1;
	for (ulCh1 = 0; ulCh1 < 2; ulCh1++) {
		gstrDaSmplChReq[ulCh1].ulChNo = ulCh1 + 1;		//チャンネル番号
		gstrDaSmplChReq[ulCh1].ulRange = DA_10V;	//チャンネルレンジ
	}

	//DAボードの設定---------------------------------------------------------------
	ULONG ulCh2;
	for (ulCh2 = 0; ulCh2 < 4; ulCh2++) {
		DaSmplChReq[ulCh2].ulChNo = ulCh2 + 1;		//チャンネル番号
		DaSmplChReq[ulCh2].ulRange = DA_10V;	//チャンネルレンジ
	}


	DaAdjustVR(ghDeviceHandle_da1, 0, 3, 4, 0);

	// ボード情報の取得（分解能など）
	nRet = AdGetDeviceInfo(hDeviceHandle_ad, &BoardSpec);
	if (nRet != AD_ERROR_SUCCESS) {
		printf("AdGetDeviceInfo errr(%lx)", nRet);
		AdClose(hDeviceHandle_ad);
		printf("error");
		return -1;
	}

	// Open a device countrer.
	dwCountMode = IFUCNT_COUNT_PHASE_4 | IFUCNT_DIR_NORMAL; //位相差パルスカウントモード 4 逓倍
	dwLoadMode = 0x10;
	dwLatchMode = 0x00;
	sprintf(szName, "IFUCNT1");
	hDeviceHandle_enc = UcntOpen(szName);
	if (hDeviceHandle_enc == INVALID_HANDLE_VALUE) {
		printf("カウンタボードのオープンに失敗しました.\n");
		DaClose(ghDeviceHandle_da2);		// DAボードは開いているはずだからクローズする。
		return -1;
	}
	counterborad();

	//AdBmSetSamplingConfig(hDeviceHandle,);

	char moter_start_key;

	do {
		cout << "Please active motor." << endl;
		cout << "If OK, press 「q」." << endl;

		cin >> moter_start_key;
		cin.clear();

	} while (moter_start_key != 'q');

	dwCounter[0] = 1000000000;
	dwCounter[1] = 1000000000;
	dwCounter[2] = 1000000000;
	dwCounter[3] = 1000000000;

	nRet = UcntSetCounter(hDeviceHandle_enc, dwChSel[0], &dwCounter[0]);
	if (nRet != IFUCNT_ERROR_SUCCESS) {
		printf("UcntSetCounter errr(%lx)\n", nRet);
	}
	nRet = UcntSetCounter(hDeviceHandle_enc, dwChSel[1], &dwCounter[0]);
	if (nRet != IFUCNT_ERROR_SUCCESS) {
		printf("UcntSetCounter errr(%lx)\n", nRet);
	}
	nRet = UcntSetCounter(hDeviceHandle_enc, dwChSel[2], &dwCounter[0]);
	if (nRet != IFUCNT_ERROR_SUCCESS) {
		printf("UcntSetCounter errr(%lx)\n", nRet);
	}
	nRet = UcntSetCounter(hDeviceHandle_enc, dwChSel[3], &dwCounter[0]);
	if (nRet != IFUCNT_ERROR_SUCCESS) {
		printf("UcntSetCounter errr(%lx)\n", nRet);
	}

	//zero_output_DA1();
	//zero_output_DA2();
	//zero_output_DA3();
	//zero_output_DA4(); //左上
	get_count_ini(0);
	get_count_ini(1);
	get_count_ini(2);
	get_count_ini(3);
	zero_output_DA();
	//get_count_ini(2); //カウンタ値読み込み
	//injector自動化準備ここまで

	//大島追加
	// // 録画用スレッドを起動
	std::thread rec_worker(recordingThreadFunc);
	rec_worker.detach(); // バックグラウンドで動作開始
	//モータ（自動インジェクタ）制御
	std::cout << "Starting Modbus motor control application..." << std::endl;
	// COMポートはご自身の環境に合わせて "COM5" などを指定してください
	sharedCtx = ModbusCVD::createSharedContext("COM5", 115200);
	if (!sharedCtx) {
		std::cerr << "Failed to create shared Modbus context" << std::endl;
		// エラーメッセージを表示して終了
		printf("Press any key to exit.");
		_getch(); // ユーザーがキーを押すまで待機
		return 1;
	}
	try {
		motor1 = new ModbusCVD(sharedCtx, 1);  // スレーブID 1
		motor2 = new ModbusCVD(sharedCtx, 2);  // スレーブID 2
		std::cout << "Motor instances created successfully." << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception occurred during motor initialization: " << e.what() << std::endl;
		ModbusCVD::closeSharedContext(sharedCtx);
		printf("Press any key to exit.");
		_getch(); // ユーザーがキーを押すまで待機
		return 1;
	}
	loadAIModel("streaming_2stream_model_cpp.pt"); // ★ AIモデルをロード
	//追加ここまで


	// サンプリング条件の取得 
	nRet = AdBmGetSamplingConfig(hDeviceHandle_ad, &SmplConfig);
	if (nRet != AD_ERROR_SUCCESS) {
		// エラー処理
		printf("AdBmGetSamplingConfig1 errr(%lx)", nRet);
		AdClose(hDeviceHandle_ad);
		exit(0);
	}

	// サンプリング件数を1件に変更 
	// サンプリングチャンネル数を2チャンネル(チャンネル1,2)に変更 

	SmplConfig.ulSmplNum = 1;
	SmplConfig.ulChCount = 2;
	SmplConfig.SmplChReq[0].ulChNo = 1;
	SmplConfig.SmplChReq[0].ulRange = AD_10V;
	SmplConfig.SmplChReq[1].ulChNo = 2;
	SmplConfig.SmplChReq[1].ulRange = AD_10V;
	SmplConfig.fSmplFreq = 100000;
	SmplConfig.fScanFreq = 200000;

	nRet = AdBmSetSamplingConfig(hDeviceHandle_ad, &SmplConfig);
	if (nRet != AD_ERROR_SUCCESS) {
		// エラー処理
		printf("AdBmSetSamplingConfig2 errr(%lx)", nRet);
		AdClose(hDeviceHandle_ad);
		exit(0);
	}

	/**/
	// チャンネル数:2
	// 指定チャンネル:CH1,CH2
	// レンジ:±5V
	AdSmplChReq[0].ulChNo = 1;
	AdSmplChReq[0].ulRange = AD_10V;
	AdSmplChReq[1].ulChNo = 2;
	AdSmplChReq[1].ulRange = AD_10V;

	malloc_for_image();
	calculate_etl2(s_dWD);
	max_expand_viewpoint_num = make_angle_table(X);
	printf("max_expand_viewpoint_num=%d, magnification=%f\n", max_expand_viewpoint_num, magnification);
	if (!QueryPerformanceFrequency(&qpf)) {
		/* error */
	}
	make_crc16_table();
	QueryPerformanceCounter(&qpc1);
	send_temperaturereading_command();
	sleep(5);
	recieve_ETL_command();
	QueryPerformanceCounter(&qpc2);

	double Time0 = ((double)qpc2.QuadPart - (double)qpc1.QuadPart) / (double)qpf.QuadPart * 1000;
	printf("Temp=%f,Time=%f\n", ETL_Temp, Time0);

	Mat frame(IMG_HEIGHT2, IMG_WIDTH2, CV_8UC1);

	sleep(10);

	for (int i = 0; i < 10; i++) {
		if (GetAsyncKeyState(0x1b)) {
			break;
		}
		cap >> img_phot;
		flip(img_phot, img_phot, 0);
		imshow("frame", img_phot);
		waitKey(1);

	}

	destroyAllWindows();

	sleep(1000);


	char c;

	do {
		cout << " 1 : Multithread_controll(ETL)" << endl
			<< " 2： Depth_observe" << endl
			//<< " 3： Edge_strength_survey" << endl
			//<< " 4 : move_by_VR" << endl
			<< " q  : Quit" << endl;


		cin >> c;
		cin.clear();

		switch (c) {
		case '1':	multithread_controll();
			break;
			//case '2':	depth_observe();
			//	break;
			//case '3':	edge_strength_survey();
			//	break;

		}

	} while (c != 'q');

	sleep(1000);

	//大島追加
	 // プログラム終了前にモーターを安全に停止させ、リソースを解放する
	if (motor1) {
		// stop関数を別スレッドで実行し、完了を待つ
		std::thread thread_stop1(stop, std::ref(*motor1), "Motor1");
		thread_stop1.join();
		delete motor1; // メモリ解放
	}
	if (motor2) {
		std::thread thread_stop2(stop, std::ref(*motor2), "Motor2");
		thread_stop2.join();
		delete motor2; // メモリ解放
	}
	ModbusCVD::closeSharedContext(sharedCtx); // 共有コンテキストを閉じる
	std::cout << "Modbus context closed." << std::endl;


	zero_output_DA();

	//zero_output_DA1();
	//act_output_DA1();// DAの全チャンネルに0Vを出力
	//zero_output_DA2();
	//act_output_DA2();// DAの全チャンネルに0Vを出力
	//zero_output_DA3();
	//act_output_DA3();// DAの全チャンネルに0Vを出力
	//zero_output_DA4();
	//act_output_DA4();// DAの全チャンネルに0Vを出力
	//act_output_DA();

	free_pointer(X);
	close_ETLCOMPort();

	/* ----Da_close--- */
	if (ghDeviceHandle_da1 != INVALID_HANDLE_VALUE) {
		nRet = DaClose(ghDeviceHandle_da1);


		if (nRet != DA_ERROR_SUCCESS) {
			printf("デバイスのクローズに失敗しました.\n");
			printf("ErrorCode=%Xh\n", nRet);	//エラーコードを表示
		}
	}
	if (ghDeviceHandle_da2 != INVALID_HANDLE_VALUE) {
		nRet = DaClose(ghDeviceHandle_da2);


		if (nRet != DA_ERROR_SUCCESS) {
			printf("デバイスのクローズに失敗しました.\n");
			printf("ErrorCode=%Xh\n", nRet);	//エラーコードを表示
		}
	}

	/*---- Ad_close ----*/
	nRet = End(hDeviceHandle_ad);
	if (nRet != 0) return -1;

	//xiCloseDevice(xiH);
	UcntClose(hDeviceHandle_enc);//カウンタクローズ

	return 0;

}