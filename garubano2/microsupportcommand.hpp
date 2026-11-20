#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

//#include<vector>
#include"Hpmcstd.h"
#pragma comment(lib, "Hpmcstd.lib")

#define RANGEDATA 1000 //出力周波数（速度）を決定する変数。速度範囲と反比例する。１０００なら 0.5～4000μｍ/s

#pragma once   // または #ifndef/#define による include guard

struct Vector3D {
    double x;
    double y;
    double z;

    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(double x_val, double y_val, double z_val) : x(x_val), y(y_val), z(z_val) {}

    // 減算演算子のオーバーロード
    Vector3D operator-(const Vector3D& other) const {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }
};

void FirstSet(HANDLE hdevice);//初期設定
void Getposition(HANDLE hdevice, double &x, double &y, double &z);
//void ChangeSpeed(HANDLE hdevice, double Highspeed[3]);//Highspeed[μｍ/s]に速度を変更
void ChangeSpeed(HANDLE hdevice, std::vector<double>Highspeed);//Highspeed[μｍ/s]に速度を変更
//void Moveto(HANDLE hdevice, double speed[3], double amounttomove[3]);//速度と動かす量を指定して動作。speedは正の値。動かす量は負の値でもOK
void Moveto(HANDLE hdevice, std::vector<double> speed, std::vector<double> amount);//速度と動かす量を指定して動作。speedは正の値。動かす量は負の値でもOK
void Moveto2(HANDLE hdevice, Vector3D speed, Vector3D amount);//速度と動かす量を指定して動作。speedは正の値。動かす量は負の値でもOK
//void Movepos(HANDLE hdevice, double speed[3], double position[3]);//speedと動かすpositionを指定して動作。speedは正の値。positionは負の値でもOK
void Movepos(HANDLE hdevice, std::vector<double> speed, std::vector<double> position);//speedと動かすpositionを指定して動作。speedは正の値。positionは負の値でもOK
void Movepos2(HANDLE hdevice, Vector3D speed, Vector3D position);//speedと動かすpositionを指定して動作。speedは正の値。positionは負の値でもOK
//void Go(HANDLE hdevice, double speed[3]);//速度と方向を指定して動作し続ける。０が正で１が負。
void Go(HANDLE hdevice, std::vector<double> speed);//速度と方向を指定して動作し続ける。０が正で１が負。
void DriveStop(HANDLE hdevice);


