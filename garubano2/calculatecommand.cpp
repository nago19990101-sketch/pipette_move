#include "calculatecommand.h"
#include "crc16ibm.h"

DWORD Ret;
HANDLE hPort;

// 送信データ生成
BYTE bSendBuffer[1024]; // 送信データ
DWORD dwSendSize; // 送信データサイズ
COMSTAT Comstat; // 通信デバイス情報構造体
DWORD dwErrorMask; // エラーコードを受け取る変数

//受信データ
BYTE In[1024];
DWORD lRead;
//int  itemp;

uint16_t _crc16 = 0;

double max_current = 292.84;
//double ETL_Temp;

void open_ETLCOMPort() {

	//シリアル通信用
	// 指定ポートを開く 
	hPort = CreateFile(
		_T("COM3"),    //　　ポートの名前： どのポートを開くのか
		GENERIC_READ | GENERIC_WRITE, //　アクセスモード：　通常送受信ともするので読書き両方を指定
		0, //　　共有モード：　通常0に設定　再オープン禁止
		NULL,  //セキュリティアトリビュート：通信では通常NULLに設定　
		OPEN_EXISTING, //　クリエイトﾃﾞｨｽﾄﾘビューション：通常COMポートはすでに存在しているのでOPEN_EXISTINGとします。
		FILE_ATTRIBUTE_NORMAL, //　属性：通信の場合属性はないのでFILE_ATTRIBUTE_NORMAL（属性なし）を指定　
		NULL  //　テンプレートのハンドル：　通信の場合関係ない　通常NULLを指定
	);
	if (hPort == INVALID_HANDLE_VALUE)  //ハンドル取得に失敗した場合
	{
		printf("ETL Port could not open.\n");
		Sleep(1000);
		exit(0);
	}

	DWORD Ret = SetupComm(     //設定
		hPort, // 　通信デバイスのハンドル：CreateFile()で取得したハンドルを指定
		1024,   //   受信バッファーサイズ：　受信のバッファーサイズをバイト単位で指定
		1024   // 　送信バッファーサイズ：　送信のバッファーサイズをバイト単位で指定
	);
	if (Ret == FALSE) //　失敗した場合
	{
		printf("SetupComm failed.\n");
		Sleep(1000);
		CloseHandle(hPort);
		exit(0);
	}

	Ret = PurgeComm( //消去
		hPort, // 　通信デバイスのハンドル：CreateFile()で取得したハンドルを指定
		PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR
		//   実行する操作： 上記は未処理の読書きの中止及び送受信のバッファーのクリアを指定
	);

	if (Ret == FALSE) //失敗した場合
	{
		printf("PurgeComm failed.\n");
		Sleep(1000);
		CloseHandle(hPort);
		exit(0);
	}

	DCB dcb;          // DCB構造体の変数を宣言
	GetCommState(hPort, &dcb); // 現在の設定値を読み込み
	dcb.DCBlength = sizeof(DCB);        //　 DCBのサイズ：
										//データ
	dcb.BaudRate = CBR_115200;                //  伝送速度：　ボーレートをbps単位で指定
	dcb.fBinary = TRUE;                 //    バイナリモード：  通常TRUEに設定
	dcb.ByteSize = 8;                    //　   データサイズ　：　通常　8 ビット
	dcb.fParity = NOPARITY;          //  パリティ有無種類　：　パリティなしの場合はNOPARITYを指定
									 //                                     奇数パリティの場合は ODDPARITY 　他
	dcb.StopBits = ONESTOPBIT;         // ストップビットの数：　通常１ビット→ ONESTOPBIT;


	//ハードウェアフロー制御
	dcb.fOutxCtsFlow = FALSE;       // 　CTSハードウェアフロー制御：CTS制御を使用しない場合はFLASEを指定
									//   　　　　　　CTS 制御をする場合はTRUEを指定してCTS信号を監視します
	dcb.fOutxDsrFlow = FALSE;       //  DSRハードウェアフロー制御：使用しない場合はFALSEを指定
	dcb.fDtrControl = DTR_CONTROL_DISABLE; // DTR有効/無効：　無効なら　DTR_CONTROL_DISABLE;ISABL
	dcb.fRtsControl = RTS_CONTROL_DISABLE;  // RTS制御：　RTS制御をしない場合はRTS_CONTROL_DISABLEを指定
									   //　　　　　　　 RTS制御をする場合はRTS_CONTROL_ENABLE　等を指定

									   // ソフトウェアフロー制御
	dcb.fOutX = FALSE;                 // 送信時XON/OFF制御の有無：　なし→FLALSE
	dcb.fInX = FALSE;                   // 受信時XON/XOFF制御の有無：なし→FALSE
	dcb.fTXContinueOnXoff = TRUE;    // 受信バッファー満杯＆XOFF受信後の継続送信可否：送信可→TRUE
	dcb.XonLim = 512;                 // XONが送られるまでに格納できる最小バイト数：
	dcb.XoffLim = 512;                 // XOFFが送られるまでに格納できる最小バイト数：
	dcb.XonChar = 0x11;                 // 送信時XON文字 ( 送信可：ビジィ解除 ) の指定：
									   //　一般に、XON文字として11H ( デバイス制御１：DC1 )がよく使われます
	dcb.XoffChar = 0x13;             // XOFF文字（送信不可：ビジー通告）の指定：なし→FALSE
									   //　一般に、XOFF文字として13H ( デバイス制御3：DC3 )がよく使われます


									   //その他

	dcb.fNull = TRUE;                 // NULLバイトの破棄： 破棄する→TRUE
	dcb.fAbortOnError = TRUE;         // エラー時の読み書き操作終了：　終了する→TRUE
	dcb.fErrorChar = FALSE;             // パリティエラー発生時のキャラクタ（ErrorChar）置換：　なし→FLALSE
	dcb.ErrorChar = 0x00;             // パリティエラー発生時の置換キャラクタ
	dcb.EofChar = 0x03;             // データ終了通知キャラクタ　：　一般に0x03(ETX)がよく使われます。
	dcb.EvtChar = 0x02;             // イベント通知キャラクタ ：　一般に0x02(STX)がよく使われます


	Ret = SetCommState(hPort, &dcb);  //SetCommState()関数にポートハンドルおよびdcb構造体のアドレスを代入する
	if (Ret == FALSE)  // 失敗した場合
	{
		printf("SetCommState failed.\n");
		//printf("%d", Ret);
		Sleep(1000);
		CloseHandle(hPort);
		exit(0);
	}

	COMMTIMEOUTS timeout; // COMMTIMEOUTS構造体の変数を宣言

	timeout.ReadIntervalTimeout = 100; // 文字読込時の時間の全体待ち時間（msec）

	timeout.ReadTotalTimeoutMultiplier = 0; //読込の１文字あたりの時間
	timeout.ReadTotalTimeoutConstant = 100; //読込エラー検出用のタイムアウト時間
											//(受信トータルタイムアウト) = ReadTotalTimeoutMultiplier * (受信予定バイト数) + ReadTotalTimeoutConstant 

	timeout.WriteTotalTimeoutMultiplier = 0; //書き込み１文字あたりの待ち時間
	timeout.WriteTotalTimeoutConstant = 100;//書き込みエラー検出用のタイムアウト時間
											//(送信トータルタイムアウト) = WriteTotalTimeoutMultiplier * (送信予定バイト数) + WriteTotalTimeoutConstant 

	Ret = SetCommTimeouts(hPort, &timeout);//SetCommTimeOut()関数にポートハンドルおよびCOMMTIMEOUTS構造体の
										   //アドレスを代入します。

	if (Ret == FALSE) //失敗した場合
	{
		printf("SetCommTimeouts failed.\n");
		Sleep(1000);
		CloseHandle(hPort);
		exit(0);
	}

}

void send_currentset_command(double current) {
	bSendBuffer[0] = 0x41;
	bSendBuffer[1] = 0x77;

	uint16_t bit_current = (uint16_t)(current / max_current * 4095);

	bSendBuffer[2] = bit_current / 0x0100;
	bSendBuffer[3] = bit_current;
	_crc16 = crc16((uint8_t*)&bSendBuffer[0], 4);
	bSendBuffer[4] = _crc16;
	bSendBuffer[5] = (_crc16 & 0xff00) / 0x0100;
	int DataSize = 6;

	ClearCommError( //  ポートの状態を取得してエラーをクリアする
		hPort, // 　通信デバイスのハンドル：CreateFile()で取得したハンドルを指定
		&dwErrorMask,//下記のエラーコードのポインタを取得→エラーの判別
					 //   CD_BREAK（フレーミングエラー）、
					 //    CE_RXPARITY（パリティエラー）、
					 //    CE_RXOVER（入力バッファーオーバーフロー）他
		&Comstat//COMSTAT構造体へのポインタを取得
				// Comstat.cbOutQue 送信バッファーの中に残っているデータ数
	);

	DWORD index = 0;
	while (DataSize > 0) {
		WriteFile(hPort,
			bSendBuffer + index, DataSize, &dwSendSize, NULL);
		index += dwSendSize;
		DataSize -= dwSendSize;
		//printf("%d\n", dwSendSize);
	}

	if (Ret == FALSE)  //失敗した場合
	{
		printf("WriteFile failed.\n");
		Sleep(1000);
		CloseHandle(hPort);
		exit(0);
	}

	if (Ret == FALSE)     //失敗した場合
	{
		Sleep(1000);
		CloseHandle(hPort);
		exit(0);
	}

};

void send_temperaturereading_command() {
	bSendBuffer[0] = 0x54;//ASCII T   ETLのシリアル通信プロトコル参照
	bSendBuffer[1] = 0x43;//ASCII C
	bSendBuffer[2] = 0x41;//ASCII A
	_crc16 = crc16((uint8_t*)&bSendBuffer[0], 3);
	bSendBuffer[3] = _crc16;
	bSendBuffer[4] = (_crc16 & 0xff00) / 0x0100;
	int DataSize = 5;
	//printf("%04x\n", hPort);
	//printf("%c%c%c%02x%02x\n", bSendBuffer[0], bSendBuffer[1], bSendBuffer[2], bSendBuffer[4], bSendBuffer[3]);


	ClearCommError( //  ポートの状態を取得してエラーをクリアする
		hPort, // 　通信デバイスのハンドル：CreateFile()で取得したハンドルを指定
		&dwErrorMask,//下記のエラーコードのポインタを取得→エラーの判別
					 //   CD_BREAK（フレーミングエラー）、
					 //    CE_RXPARITY（パリティエラー）、
					 //    CE_RXOVER（入力バッファーオーバーフロー）他
		&Comstat//COMSTAT構造体へのポインタを取得
				// Comstat.cbOutQue 送信バッファーの中に残っているデータ数
	);

	DWORD index = 0;
	while (DataSize > 0) {
		Ret = WriteFile(hPort,
			bSendBuffer + index, DataSize, &dwSendSize, NULL);
		index += dwSendSize;
		DataSize -= dwSendSize;

	}
	//printf("%d\n", index);
	if (Ret == FALSE)  //失敗した場合
	{
		printf("WriteFile failed.\n");
		Sleep(1000);
		CloseHandle(hPort);
		exit(0);
	}
	//Sleep(1);
	/*
	Ret = ReadFile(   // データの受信
		hPort,   // 　通信デバイスのハンドル：　CreateFile()で取得したハンドルを指定
		In,       // 受信バッファーのポインタを指定：　受信データがここに格納されます。
		Comstat.cbInQue, //　受信するバイト数を指定：　ここで指定するバイト数を受信するかまたはタイムアウト時間がくるまで
						 // ReadFile()関数は（　getc()のように　）待ちます
		&lRead,  //  実際に受信したバイト数（DWORD)が格納されるポインタを指定
		NULL   // 通信とは関係ない引数なのでNULLを指定
	);
	//printf("%d\n", Comstat.cbInQue);
	if (Comstat.cbInQue > 0) break;

	if (Ret == FALSE)     //失敗した場合
	{
		Sleep(1000);
		CloseHandle(hPort);
		exit(0);
	}
	*/
	//printf("%c,%c,%c,%02x,%02x\n", In[0], In[1], In[2], In[3], In[4]);

}

void recieve_ETL_command() {
	int recieveDataSize = 0;

	ClearCommError( //  ポートの状態を取得してエラーをクリアする
		hPort, // 　通信デバイスのハンドル：CreateFile()で取得したハンドルを指定
		&dwErrorMask,//下記のエラーコードのポインタを取得→エラーの判別
		//   CD_BREAK（フレーミングエラー）、
		//    CE_RXPARITY（パリティエラー）、
		//    CE_RXOVER（入力バッファーオーバーフロー）他
		&Comstat//COMSTAT構造体へのポインタを取得
		// Comstat.cbOutQue 送信バッファーの中に残っているデータ数
	);
	recieveDataSize = Comstat.cbInQue;

	//printf("Comstat.cbInQue=%d\n",Comstat.cbInQue);
	if (Comstat.cbInQue > 0) {
		while (recieveDataSize > 0) {
			Ret = ReadFile(   // データの受信
				hPort,   // 　通信デバイスのハンドル：　CreateFile()で取得したハンドルを指定
				In,       // 受信バッファーのポインタを指定：　受信データがここに格納されます。
				Comstat.cbInQue, //　受信するバイト数を指定：　ここで指定するバイト数を受信するかまたはタイムアウト時間がくるまで
				// ReadFile()関数は（　getc()のように　）待ちます
				&lRead,  //  実際に受信したバイト数（DWORD)が格納されるポインタを指定
				NULL   // 通信とは関係ない引数なのでNULLを指定
			);

			recieveDataSize = recieveDataSize - lRead;
			//printf("recieveDataSize=%d\n",recieveDataSize);
		}

	}

	if (Ret == FALSE)     //失敗した場合
	{
		printf("Error_Recieve_ETL_command");
		Sleep(1000);
		CloseHandle(hPort);
		exit(0);
	}
	//printf("%c,%c,%c,%c,%c\n",In[0],In[1],In[2],In[3],In[4]);
	if (In[0] == 0x54) ETL_Temp = ((double)In[3] * 0x0100 + (double)In[4]) * 0.0625;

}

void close_ETLCOMPort() {
	CloseHandle(hPort);
}

