#pragma once

//#include "stdafx.h"
#include "windows.h"
#include "string"
#include <tchar.h>

/*
extern HANDLE hPort;

// 送信データ生成
extern BYTE bSendBuffer[1024]; // 送信データ
extern DWORD dwSendSize; // 送信データサイズ
extern COMSTAT Comstat; // 通信デバイス情報構造体
extern DWORD dwErrorMask; // エラーコードを受け取る変数

//受信データ
extern BYTE In[1024];
extern DWORD lRead;
extern int  itemp;

extern DWORD Ret;
*/
extern double ETL_Temp;
extern double max_current;


void open_ETLCOMPort();
void send_currentset_command(double current);
void send_temperaturereading_command();
void recieve_ETL_command();
void close_ETLCOMPort();