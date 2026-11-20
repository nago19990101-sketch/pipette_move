#pragma once
//#include <Function.h>
#include <windows.h>
#include "opencv2/opencv.hpp"
#include <sstream>
#include <stdio.h>
#include <iostream>
using namespace cv;

#define PI				3.1415		// πの定義
#define  nstoreimage 300//1024
#define max_pointed_count 4
#define  nstorepointedimage nstoreimage*max_pointed_count

extern double** plist;            /* ポインタへのポインタplistを宣言 */
extern double** pangle;            /* ポインタへのポインタplistを宣言 */
extern double** ppointedangle;            /* ポインタへのポインタplistを宣言 */
extern unsigned char** pimage;            /* ポインタへのポインタplistを宣言 */
extern unsigned char** ppointedimage;            /* ポインタへのポインタplistを宣言 */
extern int ndata[30];

extern double magnification; //倍率
extern double pixelpitch;//ピクセルピッチ
#define IMG_WIDTH    	1246//1246//640 //画像の大きさ（横）
#define IMG_HEIGHT		1008//1024//480	//画像の大きさ（縦）


extern double l1;//対物レンズからミラー距離 mm
extern double l2;//ミラーからミラーまでの距離 mm
extern double l3;  //ミラーから平面までの距離 mm
extern double D_ob;//観察対象の厚さμm
extern double DoF_f;//Dopt of Focus front
extern double DoF_b;//Dopt of Focus back

double qua(double a, double b, double c, int s);
double dsin(double deg);
double dcos(double deg);
double dtan(double deg);
void contrast2(unsigned char* in_imageaddress, unsigned char* out_imageaddress);
void contrast(unsigned char* in_imageaddress);
double derive_xangle(double xangle);
double derive_yangle(double yangle);
void derive_angle(double gamma, double* xangle, double* yangle, double WD);
void derive_yangle2(double* xangle, double* yangle, double WD);
double derive_WD(double xangle, double yangle);
int store_image_angle(double* xnagle, double* yangle, unsigned char* imageaddress, double* storeangleaddres, unsigned char* storeimageaddress);
void create_image(double zin, double* storeangleaddress, unsigned char* storeimageaddress, Mat image, Mat Depth, double WD, double resolution);
void create_image2(double zin, double* storeangleaddress, unsigned char* storeimageaddress, Mat image, Mat Depth, double WD);
int make_angle_table(int X);
void malloc_for_image();
void free_pointer(int X);
void create_img(unsigned char* storeimageaddress, Mat image);