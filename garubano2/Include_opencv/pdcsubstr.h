/*
 *	PDCSUBSTR.h
 *	フォトロンカメラデバイスコントロールSDK
 *	デバイス状態構造体定義ファイル
 *
 *	Copyright (C) 2006-2007 PHOTRON LIMITED
 */

#include	"PDCVALUE.h"
#include	"PDCSTR.h"

#ifndef	_PDC_SUB_STR_H_
#define	_PDC_SUB_STR_H_

#pragma	pack(4)
/*
 *	子デバイス(ヘッド)記録情報構造体
 */
typedef struct {
	unsigned long	m_nRecordRate;		/*	撮影速度	*/
	unsigned long	m_nWidth;			/*	幅	*/
	unsigned long	m_nHeight;			/*	高さ	*/
	unsigned long	m_nShutterSpeedFps;	/*	シャッター速度（1/fps）	*/
	unsigned long	m_nShutterSpeedUsec;/*	シャッター速度（usec）	*/
	unsigned long	m_nTriggerMode;		/*	トリガモード	*/
	unsigned long	m_nTriggerAfterFrames;/*	トリガ後フレーム設定枚数	*/
	unsigned long	m_nTriggerRandomFrames;/*	ランダムトリガ設定枚数	*/
	unsigned long	m_nTriggerRandomCount;/*	ランダムトリガ回数	*/
	unsigned long	m_nEdgeEnhance;		/*	エッジエンハンス	*/
	unsigned long	m_nMCDL;			/*	MCDL	*/
	unsigned long	m_nIRIG;			/*	IRIG	*/
	unsigned long	m_nSensorGain;		/*	センサーゲイン	*/
	unsigned long	m_nSensorGamma;		/*	センサーガンマ	*/
	unsigned long	m_nDSShutter;		/*	DSシャッター	*/
	unsigned long	m_nDSShutterValue;	/*	DSシャッター値	*/
	unsigned long	m_nColorTempMode;	/*	色温度	*/
	unsigned long	m_nColorTempR;		/*	色温度R	*/
	unsigned long	m_nColorTempG;		/*	色温度G	*/
	unsigned long	m_nColorTempB;		/*	色温度B	*/
	unsigned long	m_nAutoExposure;	/*	オートエクスポージャー	*/
	unsigned long	m_nSegmentXPos;		/*	センサー内ポジション左上座標X	*/
	unsigned long	m_nSegmentYPos;		/*	センサー内ポジション左上座標Y	*/
	unsigned long	m_nLiveResolution;	/*	Live Resolution 20080419: Cuong Add	*/
	long			m_nColorEnhanceRR;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceRG;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceRB;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceGR;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceGG;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceGB;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceBR;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceBG;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceBB;			/*	カラーエンハンス値	*/
	unsigned long	m_nColorEnhance;			/*	カラーエンハンス	*/
	unsigned long	m_nIIPower;				/*	I.I.電源	*/
	unsigned long	m_nIIGain;				/*	I.I.ゲイン	*/
	unsigned long	m_nIIGate;				/*	I.I.ゲートモード	*/
	unsigned long	m_nIIGateCycle;			/*	I.I.ゲートサイクル	*/
	unsigned long	m_nIIGateWidth;			/*	I.I.ゲート幅	*/
	unsigned long	m_nIIGateDelay;			/*	I.I.ゲートディレイ	*/
	unsigned long	m_nLUTMode;					/*	LUTモード	*/
	unsigned long	m_nIRIGPhaseLockStatus;	/*	IRIG同期動作モード	*/
	PDC_FRAME_INFO	m_FrameInfo;		/*	フレーム情報	*/
//					m_RecordedTime;	/*	撮影時刻	*/
} PDC_CHILDDEVICE_MEMORY_INFO, *PPDC_CHILDDEVICE_MEMORY_INFO;


/*
 *	子デバイス(ヘッド)情報構造体
 */
typedef struct {
	unsigned long	m_nRecordRate;		/*	撮影速度	*/
	unsigned long	m_nWidth;			/*	幅	*/
	unsigned long	m_nHeight;			/*	高さ	*/
	unsigned long	m_nMaxWidth;		/*	最大幅	*/
	unsigned long	m_nMaxHeight;		/*	最大高さ	*/
	unsigned long	m_nShutterSpeedFps;	/*	シャッター速度（1/fps）	*/
	unsigned long	m_nShutterSpeedUsec;/*	シャッター速度（usec）	*/
	unsigned long	m_nEdgeEnhance;		/*	エッジエンハンス	*/
	unsigned long	m_nSensorGain;		/*	センサーゲイン	*/
	unsigned long	m_nSensorGamma;		/*	センサーガンマ	*/
	unsigned long	m_nDSShutter;		/*	DSシャッター	*/
	unsigned long	m_nDSShutterValue;	/*	DSシャッター値	*/
	unsigned long	m_nColorTempMode;	/*	色温度	*/
	unsigned long	m_nColorTempR;		/*	色温度R	*/
	unsigned long	m_nColorTempG;		/*	色温度G	*/
	unsigned long	m_nColorTempB;		/*	色温度B	*/
	unsigned long	m_nAutoExposure;	/*	オートエクスポージャー	*/
	unsigned long	m_nIIPower;				/*	I.I.電源	*/
	unsigned long	m_nIIGain;				/*	I.I.ゲイン	*/
	unsigned long	m_nIIGate;				/*	I.I.ゲートモード	*/
	unsigned long	m_nIIGateCycle;			/*	I.I.ゲートサイクル	*/
	unsigned long	m_nIIGateWidth;			/*	I.I.ゲート幅	*/
	unsigned long	m_nIIGateDelay;			/*	I.I.ゲートディレイ	*/
	char			m_strChildName[PDC_MAX_STRING_LENGTH];		/*	ヘッド名称	*/
	unsigned long	m_nProductID;				/*	子デバイスプロダクトNo.(PCI Type)	*/
	unsigned long	m_nLotID;					/*	子デバイスロットNo.(PCI Type)	*/
	unsigned long	m_nIndividualID;			/*	子デバイス個別No.(PCI Type)	*/
	unsigned long	m_nVersion;					/*	子デバイスファーム/FPGAバージョン	*/
	unsigned long	m_nShadingMode;				/*	シェーディング	*/
	unsigned long	m_nPixelGainMode;			/*	ピクセルゲイン	*/
	unsigned long	m_nMaxVariableChannel;		/*	最大バリアブルチャンネル数	*/
	unsigned long	m_nCurrentVariableChannel;	/*	現在のバリアブルチャンネル	*/
	unsigned long	m_nMaxPartitionCount;		/*	作成可能な最大パーティション数	*/
	unsigned long	m_nMaxPartitionBlockCount;	/*	作成可能な最大パーティションブロック数	*/
	unsigned long	m_nPartitionCount;			/*	パーティション数	*/
	unsigned long	m_nCurrentPartition;		/*	カレントパーティション	*/
	unsigned long	m_nPartitionBlock;			/*	パーティションブロック数	*/
	unsigned long	m_nMaxFrameCount;			/*	撮影可能なフレーム枚数	*/
	unsigned long	m_nColorType;				/*	カラータイプ	*/
	unsigned long	m_nIIGainLimit;				/*	I.I.ゲイン下限値	*/
	unsigned long	m_nIIWidthLimit;			/*	I.I.ゲート幅下限値	*/
	unsigned long	m_nLUTMode;					/*	LUTモード	*/
	unsigned long	m_nSegmentXPos;				/*	センサー内ポジション左上座標X	*/
	unsigned long	m_nSegmentYPos;				/*	センサー内ポジション左上座標Y	*/
	unsigned long	m_nMaxBitDepth;				/*	最大ビット深度	*/
	unsigned long	m_nMaxFullResoRate;			/*	最大解像度時における最高速撮影速度	*/
	unsigned long	m_nCamMode;					/*	カメラモード	*/	
	unsigned long	m_nLiveResolution;			/*	Live Resolution	20080419: Cuong Add*/
	long			m_nColorEnhanceRR;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceRG;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceRB;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceGR;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceGG;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceGB;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceBR;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceBG;			/*	カラーエンハンス値	*/
	long			m_nColorEnhanceBB;			/*	カラーエンハンス値	*/
	unsigned long	m_nColorEnhance;			/*	カラーエンハンス	*/
	unsigned long	m_nMaxColorTemp;			/*	色温度設定最大値	*/
	unsigned long	m_nBaseColorTemp;			/*	色温度設定基準値	*/
	char			m_ExistSensorGain;			/*	センサーゲイン	*/
	char			m_ExistSensorGamma;			/*	センサーガンマ	*/
	char			m_ExistColorTemp;			/*	色温度	*/
	char			m_ExistDSShutter;			/*	DSシャッター	*/
	char			m_ExistShading;				/*	シェーディング	*/
	char			m_ExistPixelGain;			/*	ピクセルゲイン	*/
	char			m_ExistVariableFunction;	/*	バリアブルレート・解像度	*/
	char			m_ExistColorEnhance;		/*	カラーエンハンス	*/
	char			m_ExistIIFunction;			/*	I.I.制御	*/
	char			m_ExistIIProtection;		/*	I.I.保護機能	*/
	char			m_ExistAutoExposure;		/*	オートエクスポージャー	*/
	char			m_ExistVariablePartition;	/*	パーティションブロック指定分割	*/
	char			m_ExistLUT;					/*	LUT	*/
	char			m_ExistEdgeEnhance;			/*	エッジエンハンス	*/
	char			m_ExistShutterSpeedUsec;	/*	シャッター速度(usec)	*/
	char			m_ExistLiveResolution;		/*	Live Resolution	20080419: Cuong Add	*/
	PDC_CHILDDEVICE_MEMORY_INFO	m_MemoryInfo;	/*	カレントパーティション情報	*/
} PDC_CHILDDEVICE_INFO, *PPDC_CHILDDEVICE_INFO;

/*
 *	デバイス情報構造体
 */
typedef struct {
	PDC_DETECT_INFO	m_DetectInfo;			/*	デバイス検索構造体	*/
	unsigned long	m_nModuleHandle;		/*	モジュールハンドル	*/
	unsigned long	m_nDeviceHandle;		/*	デバイスハンドル	*/
	char			m_strDeviceName[PDC_MAX_STRING_LENGTH];	/*	デバイス名称	*/
	unsigned long	m_nDeviceID;			/*	デバイスID	*/
	unsigned long	m_nProductID;			/*	本体プロダクトNo.(PCI Type)	*/
	unsigned long	m_nLotID;				/*	本体ロットNo.(PCI Type)	*/
	unsigned long	m_nIndividualID;		/*	本体個別No.(PCI Type)	*/
	unsigned long	m_nVersion;				/*	本体ファーム/FPGAバージョン	*/
	unsigned long	m_nExternalIn[PDC_EXTIO_MAX_PORT];	/*	外部入力	*/
	unsigned long	m_nExternalOut[PDC_EXTIO_MAX_PORT];	/*	外部出力	*/
	unsigned long	m_nExternalInCount;		/*	外部入力ポート数	*/
	unsigned long	m_nExternalOutCount;	/*	外部出力ポート数	*/
	unsigned long	m_nSyncOutTimes;		/*	外部同期信号出力倍率	*/
	unsigned long	m_nShutterLockMode;		/*	シャッター速度固定モード	*/
	unsigned long	m_nPartitionIncMode;	/*	パーティションインクリメントモード	*/
	unsigned long	m_nChildDeviceCount;	/*	子デバイス(ヘッド)の数	*/
	unsigned long	m_nMaxChildDeviceCount;	/*	最大子デバイス(ヘッド)の数	*/
	unsigned long	m_nKeyLock;				/*	キーパッドロック	*/
	unsigned long	m_nVideoOut;			/*	ビデオアウト(NTSC/PAL)	*/
	unsigned long	m_nTriggerMode;			/*	トリガモード	*/
	unsigned long	m_nTriggerAfterFrames;	/*	トリガ後フレーム設定枚数	*/
	unsigned long	m_nTriggerRandomFrames;	/*	ランダムトリガ設定枚数	*/
	unsigned long	m_nTriggerRandomCount;	/*	ランダムトリガ回数	*/
	unsigned long	m_nIRIG;				/*	IRIG	*/
	unsigned long	m_nMCDL;				/*	MCDL	*/
	long			m_nIRIGOffset;			/*	IRIGオフセット	*/
	unsigned long	m_nIRIGPhaseLock;		/*	IRIG同期モード	*/
	unsigned long	m_nIRIGPhaseLockStatus;	/*	IRIG同期動作モード	*/
	unsigned long	m_nDelayLimitTriggerIn;	/*	ディレイリミット(TGIn)*/
	unsigned long	m_nDelayLimitSyncIn;	/*	ディレイリミット(SyncIn)*/
	unsigned long	m_nDelayLimitGeneralIn;	/*	ディレイリミット(GenaralIn)	*/
	unsigned long	m_nDelayLimitTriggerOutWidth;	/*	ディレイリミット(TGOut Width)	*/
	unsigned long	m_nDelayLimitSyncOut;	/*	ディレイリミット(SyncOut)	*/
	unsigned long	m_nDelayLimitSyncOutWidth;	/*	ディレイリミット(SyncOut Width)	*/
	unsigned long	m_nDelayLimitExpose;	/*	ディレイリミット(Expose)	*/
	unsigned long	m_nDelayTriggerIn;		/*	ディレイ(TGIn)	*/
	unsigned long	m_nDelaySyncIn;			/*	ディレイ(SyncIn)	*/
	unsigned long	m_nDelayGeneralIn;		/*	ディレイ(GenaralIn)	*/
	unsigned long	m_nDelayTriggerOutWidth;	/*	ディレイ(TGOut Width)	*/
	unsigned long	m_nDelaySyncOut;		/*	ディレイ(SyncOut)	*/
	unsigned long	m_nDelaySyncOutWidth;	/*	ディレイ(SyncOut Width)	*/
	unsigned long	m_nDelayExpose;			/*	ディレイ(Expose)	*/
	unsigned long	m_nDownloadMode;		/*	ダウンロードモード	*/
	unsigned long	m_nLowLightMode;		/*	ローライトモード	*/
	unsigned long	m_nProgSwitchMode[PDC_PROGSWITCH_MAX_NUM];		/*	本体プログラマブルスイッチモード	*/
	unsigned long	m_nStoreNum;			/*	STOREプリセット数	*/
	unsigned long	m_nHighSpeedMode;		/*	高速動作モード	*/
	char			m_ExistShutterLockMode;	/*	シャッター速度固定モード	*/
	char			m_ExistPartitionIncMode;/*	パーティションインクリメントモード	*/
	char			m_ExistHeadExchange;	/*	ヘッド交換機能	*/
	char			m_ExistKeyLock;			/*	キーパッドロック	*/
	char			m_ExistVideoOut;		/*	ビデオアウト(NTSC/PAL)	*/
	char			m_ExistIRIG;			/*	IRIG	*/
	char			m_ExistMCDL;			/*	MCDL	*/
	char			m_ExistEventFrame;		/*	イベントフレーム	*/
	char			m_ExistFrameZero;		/*	フレーム番号0	*/
	char			m_ExistPartitionBlock;	/*	パーティションブロック自由分割機能	*/
	char			m_ExistDelay;			/*	ディレイ	*/
	char			m_ExistDownloadMode;		/*	ダウンロードモード	*/
	char			m_ExistVariableFunction;	/*	バリアブル機能	*/
	char			m_ExistLowLightMode;	/*	ローライトモード	*/
	char			m_ExistProgSwitchMode;	/*	プログラマブルスイッチモード	*/
	char			m_ExistSyncOutTimes;	/*	同期信号出力倍率	*/
	char			m_ExistStore;			/*	STORE機能	*/
	char			m_ExistEtherInfo;		/*	イーサネット設定変更	*/
	char			m_ExistIRIGPhaseLock;	/*	IRIG同期モード	*/
	char			m_ExistHighSpeedMode;	/*	高速動作モード	*/
	char			m_ExistBurstTransfer;	/*	バースト転送モード	*/
	char			m_ExistStepShutter;		/*	シャッター速度ステップ刻み	*/
	PPDC_CHILDDEVICE_INFO	m_ChildInfo[PDC_MAX_CHILD_DEVICE];	/*	ヘッド情報	*/
} PDC_DEVICE_INFO, *PPDC_DEVICE_INFO;

#pragma pack()
#endif	/*	_PDC_STR_H_	*/

