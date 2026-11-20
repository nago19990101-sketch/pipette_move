/*
 *	PDCSubFunc.h
 *	フォトロンカメラデバイスコントロールSDK
 *	サブ命令定義ファイル
 *
 *	Copyright (C) 2006 PHOTRON LIMITED
 *	2006.02.20	Takayuki NAKAMURA
 */

#ifndef	_PDC_SUB_FUNC_H_
#define	_PDC_SUB_FUNC_H_

/*	DeviceCaps命令コード	*/
enum {
	PDC_CAPS_STATUS = 0x0000,		/*	ステータス	*/
	PDC_CAPS_CHILD_DEVICE_COUNT,	/*	子デバイス数	*/
	PDC_CAPS_RECORDRATE,			/*	撮影レート	*/
	PDC_CAPS_RESOLUTION,			/*	解像度	*/
	PDC_CAPS_SHUTTER_SPEED_FPS,		/*	シャッター速度(FPS)	*/
	PDC_CAPS_SHUTTER_SPEED_USEC,	/*	シャッター速度(usec)	*/
	PDC_CAPS_TRIGGER_MODE,			/*	トリガモード	*/
	PDC_CAPS_RANDOM_TRIGGER_COUNT,	/*	ランダムトリガ回数	*/
	PDC_CAPS_COLOR_TEMP,			/*	色温度	*/
	PDC_CAPS_SENSOR_GAIN,			/*	センサーゲイン	*/
	PDC_CAPS_SENSOR_GAMMA,			/*	センサーガンマ	*/
	PDC_CAPS_IS_COLOR,				/*	カラー/モノクロ	*/
	PDC_CAPS_DEVICE_ID,				/*	デバイスID	*/
	PDC_CAPS_EDGE_ENHANCE,			/*	エッジエンハンス	*/
	PDC_CAPS_IRIG,					/*	IRIG	*/
	PDC_CAPS_MCDL,					/*	MCDL	*/
	PDC_CAPS_SHADING,				/*	シェーディング	*/
	PDC_CAPS_DS_SHUTTER_MODE,		/*	DSシャッター	*/
	PDC_CAPS_LUT_MODE,				/*	LUT	*/
	PDC_CAPS_IRIG_OFFSET,			/*	IRIGオフセット	*/
	PDC_CAPS_PRODUCT_ID,			/*	プロダクトID	*/
	PDC_CAPS_LOT_ID,				/*	ロットID	*/
	PDC_CAPS_INDIVIDUAL_ID,			/*	固有ID	*/
	PDC_CAPS_VERSION,				/*	ファームウェア/FPGAバージョン	*/
	PDC_CAPS_SHUTTER_LOCK_MODE,		/*	シャッター固定モード	*/
	PDC_CAPS_PARTITION_INC_MODE,	/*	パーティションインクリメントモード	*/
	PDC_CAPS_HEAD_EXCHANGE_COUNT,	/*	ヘッドパターン数	*/
	PDC_CAPS_HEAD_EXCHANGE_NO,		/*	ヘッドパターンナンバー	*/
	PDC_CAPS_KEYPAD_LOCK,			/*	キーパッドロック	*/
	PDC_CAPS_VIDEO_OUT,				/*	ビデオアウト(NTSC/PAL)	*/
	PDC_CAPS_MAX_PARTITION,			/*	最大パーティション数	*/
	PDC_CAPS_CURRENT_PARTITION,		/*	操作対象パーティション	*/
	PDC_CAPS_PIXELGAIN,				/*	ピクセルゲイン	*/
	PDC_CAPS_MAX_FRAME,				/*	最大録画枚数	*/
	PDC_CAPS_EXTERNAL_IN,			/*	外部入力	*/
	PDC_CAPS_EXTERNAL_OUT,			/*	外部出力	*/
	PDC_CAPS_EXTERNAL_COUNT,		/*	外部入出力ポート数	*/
	PDC_CAPS_AUTO_EXPOSURE,			/*	オートエクスポージャー	*/
	PDC_CAPS_FRAME_ZERO,			/*	フレームゼロ	*/
	PDC_CAPS_PARTITION_LIST,		/*	パーティションリスト	*/
	PDC_CAPS_PARTITION_COUNT,		/*	現在のパーティション数	*/
	PDC_CAPS_TRANSFER_OPTION,		/*	転送オプション	*/
	PDC_CAPS_RECORDED_FRAME,		/*	録画フレーム枚数	*/
	PDC_CAPS_TWOSTAGE_FRAME,		/*	ツーステージフレーム情報	*/
	PDC_CAPS_COLOR_ENHANCE_R,		/*	カラーマトリクスR	*/
	PDC_CAPS_COLOR_ENHANCE_G,		/*	カラーマトリクスG	*/
	PDC_CAPS_COLOR_ENHANCE_B,		/*	カラーマトリクスB	*/
	PDC_CAPS_RESOLUTION_MAX,		/*	最大解像度	*/
	PDC_CAPS_SEGMENT_POS,			/*	センサー内セグメント左上座標	*/
	PDC_CAPS_CAM_MODE,				/*	カメラモード(Variable)	*/
	PDC_CAPS_VARIABLE_INFO_RATE,	/*	バリアブル情報(レート)	*/
	PDC_CAPS_VARIABLE_INFO_RESO,	/*	バリアブル情報(解像度)	*/
	PDC_CAPS_VARIABLE_INFO_POS,		/*	バリアブル情報(座標)	*/
	PDC_CAPS_VARIABLE_INFO,			/*	バリアブル情報(設定)	*/
	PDC_CAPS_VARIABLE_CHANNEL,		/*	バリアブルチャンネルセレクト(ロード)	*/
	PDC_CAPS_VARIABLE_ERASE,		/*	バリアブル消去	*/
	PDC_CAPS_VARIABLE_MAX_RATE,		/*	バリアブル最大レート	*/
	PDC_CAPS_VARIABLE_MAX_WIDTH,	/*	バリアブル最大幅	*/
	PDC_CAPS_VARIABLE_MAX_HEIGHT,	/*	バリアブル最大高さ	*/
	PDC_CAPS_VARIABLE_MAX_RESO,		/*	バリアブル最大解像度(正方)	*/
	PDC_CAPS_AE_INFO_RESO,			/*	AE情報(解像度)	*/
	PDC_CAPS_AE_INFO_POS,			/*	AE情報(座標)	*/
	PDC_CAPS_AE_INFO_VALUE,			/*	AE情報(値)		*/
	PDC_CAPS_AE_INFO_RANGE,			/*	AE情報(許容幅)	*/
	PDC_CAPS_AE_INFO_LIMIT,			/*	AE情報(最大露光)	*/
	PDC_CAPS_AE_INFO,				/*	AE情報(設定)	*/
	PDC_CAPS_MAX_CHILD_DEVICE_COUNT,	/*	最大子デバイス数	*/
	PDC_CAPS_RECORDRATE_CACHE,		/*	撮影レート(実アクセスなし)	*/
	PDC_CAPS_RESOLUTION_CACHE,		/*	解像度(実アクセスなし)	*/
	PDC_CAPS_MAX_FRAME_CACHE,		/*	最大録画枚数(実アクセスなし)	*/
	PDC_CAPS_RECORDED_FRAME_CACHE,	/*	録画フレーム枚数(実アクセスなし)	*/
	PDC_CAPS_SEGMENT_POS_CACHE,		/*	センサー内セグメント左上座標(実アクセスなし)	*/
	PDC_CAPS_MAX_BITDEPTH,			/*	デバイス最大ビット深度	*/
	PDC_CAPS_LUT_UPLOAD,			/*	LUT USER値のアップロード	*/
	PDC_CAPS_DELAY,					/*	ディレイ設定	*/
	PDC_CAPS_DELAY_LIMIT,			/*	ディレイ限界値設定	*/
	PDC_CAPS_VARIABLE_RESTRICTION,	/*	バリアブル設定の制限	*/
	PDC_CAPS_COLOR_ENHANCE,			/*	カラーマトリクスOn/Off	*/
	PDC_CAPS_DOWNLOAD_MODE,			/*	ダウンロードモード	*/
	PDC_CAPS_CAM_MODE_CACHE,		/*	カメラモード（実アクセスなし）	*/
	PDC_CAPS_VARIABLE_FUNCTION,		/*	バリアブル機能の有無	*/
	PDC_CAPS_MAX_FULLRESO_RATE,		/*	最大解像度時における最高速撮影速度	*/
	PDC_CAPS_LOWLIGHT_MODE,			/*	ローライトモード	*/
	PDC_CAPS_PROGSWITCH_MODE,		/*	プログラマブルボタンモード	*/
	PDC_CAPS_TWOSTAGE_TIMING,		/*	ツーステージタイミング情報	*/
	PDC_CAPS_PIXELDEF_REVISION,		/*	画素欠損	*/
	PDC_CAPS_COLOR_TEMP_R,			/*	色温度R	*/
	PDC_CAPS_COLOR_TEMP_G,			/*	色温度G	*/
	PDC_CAPS_COLOR_TEMP_B,			/*	色温度B	*/
	PDC_CAPS_COLOR_TEMP_BASE,		/*	色温度デフォルト値	*/
	PDC_CAPS_TRANSFER_PIX_BUF,		/*	画像転送時に画素値取得用バッファにコピー	*/
	PDC_CAPS_AE_RESTRICTION,		/*	AE設定の制限	*/
	PDC_CAPS_BAYERCONVERT,			/*	ベイヤー変換モード	*/
	PDC_CAPS_DS_SHUTTER_VALUE,		/*	DSシャッター値	*/
	PDC_CAPS_SYNCOUT_TIMES,			/*	外部同期出力倍率	*/
	PDC_CAPS_STORE,					/*	STORE動作	*/
	PDC_CAPS_STORE_NUM,				/*	STOREプリセット数	*/
	PDC_CAPS_ETHER_INFO,			/*	イーサネット設定	*/
	PDC_CAPS_VARIABLE_INFO_IRIG,	/*	バリアブル情報(IRIG同期)	*/
	PDC_CAPS_IRIG_PHASELOCK,		/*	IRIG同期モード	*/
	PDC_CAPS_IRIG_PHASELOCK_STATUS,	/*	IRIG同期動作モード	*/
	PDC_CAPS_HIGH_SPEED_MODE,		/*	高速動作モード	*/
	PDC_CAPS_BURST_TRANSFER,		/*	バースト転送モード	*/
	PDC_CAPS_VARIABLE_INFO_HSMODE,	/*	バリアブル情報(高速モード)	*/
	PDC_CAPS_HEAD_EXCHANGE,			/*	HEAD EXCHANGE	*/
	PDC_CAPS_STEP_SHUTTER,			/*	ステップシャッター	*/
	PDC_CAPS_RESET_FPGA,			/* Reset FPGA after configured*/
	PDC_CAP_SIZE_XSVF_FILE,
	/*Extern Function*/
	PDC_CAPS_CAM_STATUS,
	PDC_CAPS_LIVE_RES0LUTION,		/*	LIVE RESOLUTION	*/
	PDC_CAPS_MEMORY,
	//<--20081120: Nha add
	PDC_CAPS_RECORDED_FRAME_PER_TRIGGER,
	//-->
	//<--20090210
	PDC_CAPS_MEMORY_AREA,
	//-->
};
#define PDC_CAPS_FPGA_CONF_STATUS 300
/*	リスト命令コード	*/
enum {
	PDC_LIST_RECORDRATE = 0x0000,	/*	撮影レート	*/
	PDC_LIST_RESOLUTION,			/*	解像度	*/
	PDC_LIST_SHUTTER_SPEED_FPS,		/*	シャッター速度(FPS)	*/
	PDC_LIST_SHUTTER_SPEED_USEC,	/*	シャッター速度(usec)	*/
	PDC_LIST_TRIGGER_MODE,			/*	トリガモード	*/
	PDC_LIST_SENSOR_GAIN,			/*	センサーゲイン	*/
	PDC_LIST_SENSOR_GAMMA,			/*	センサーガンマ	*/
	PDC_LIST_EXTERNAL_IN,			/*	外部入力	*/
	PDC_LIST_EXTERNAL_OUT,			/*	外部出力	*/
	PDC_LIST_COLOR_TEMP,			/*	色温度	*/
	PDC_LIST_LUT_MODE,				/*	LUT	*/
	PDC_LIST_VARIABLE_RECORDRATE,	/*	バリアブル撮影レート	*/
	PDC_LIST_DS_SHUTTER_MODE,		/*	DSシャッター	*/
	PDC_LIST_VIDEO_OUT,				/*	ビデオアウト(NTSC/PAL)	*/
	PDC_LIST_SHADING,				/*	シェーディング	*/
	PDC_LIST_AE_SHUTTER_FPS,		/*	Auto Exposureシャッター(FPS)	*/
	PDC_LIST_AE_SHUTTER_USEC,		/*	Auto Exposureシャッター(usec)	*/
	PDC_LIST_EDGE_ENHANCE,			/*	エッジエンハンス	*/
	PDC_LIST_PIXEL_DEF,				/*	画素欠損座標	*/
	PDC_LIST_CHILDDEVICE_EXIST,		/*	有効ヘッド番号リスト	*/
	PDC_LIST_LUT_VALUE_R,			/*	LUT 色温度補正R	*/
	PDC_LIST_LUT_VALUE_G,			/*	LUT 色温度補正G	*/
	PDC_LIST_LUT_VALUE_B,			/*	LUT 色温度補正B	*/
	PDC_LIST_EVENT_FRAME,			/*	イベントフレーム	*/
	PDC_LIST_PROGSWITCH_MODE,		/*	プログラマブルボタンモード	*/
	PDC_LIST_LUT_VALUE_ORG_R,		/*	LUT 実値R	*/
	PDC_LIST_LUT_VALUE_ORG_G,		/*	LUT 実値R	*/
	PDC_LIST_LUT_VALUE_ORG_B,		/*	LUT 実値R	*/
	PDC_LIST_PIXEL_GAIN,			/*	ピクセルゲイン	*/
	PDC_LIST_SYNCOUT_TIMES,			/*	外部同期出力倍率	*/
	PDC_LIST_EVENT_RCOUNT,			/*	イベントフレームランダム回数	*/
	PDC_LIST_COLOR_ENHANCE,			/*	カラーエンハンス	*/
	PDC_LIST_HEAD_EXCHANGE,			/*	HEAD EXCHANGE	*/
	PDC_LIST_LIVE_RESOLUTION,		/*	LIVE RESOLUTION 20080419: Cuong	*/
};

/*	転送データコード	*/
enum {
	PDC_DATA_IRIG = 0,			/*	IRIG	*/
	PDC_DATA_MCDL,				/*	MCDL	*/
	PDC_DATA_IRIGMCDL,			/*	IRIG, MCDL	*/
	PDC_DATA_LUT_USER_PARAMS,	/*	LUTパラメータ	*/
	PDC_DATA_PIXEL_GAIN,		/*	ピクセルゲインデータ	*/
	PDC_DATA_PIXEL_GAIN_CALIBRATION,	/*	ピクセルゲインキャリブレーション	*/
	PDC_DATA_PIXEL_VALUE,		/*	画素値	*/
	PDC_DATA_CAMERA_CONTROL,	// Camera control debug
};


/*	バリアブル制限要求コード	*/
enum {
	PDC_VARIABLE_REST_POS_STEP = 0,		/*	座標移動最小単位	*/
	PDC_VARIABLE_REST_RESO_STEP,		/*	解像度変動最小単位	*/
	PDC_VARIABLE_REST_RESO,				/*	最小解像度	*/
	PDC_VARIABLE_REST_FREEPOS,			/*	フリーポジション	*/
};

/*	オートエクスポージャー制限要求コード	*/
enum {
	PDC_AE_REST_POS_STEP = 0,	/*	座標移動最小単位	*/
	PDC_AE_REST_RESO_STEP,		/*	解像度変動最小単位	*/
	PDC_AE_REST_RESO,			/*	最小解像度	*/
	PDC_AE_REST_VALUE,			/*	設定値最大	*/
};

/*	ステップシャッター要求コード	*/
enum {
	PDC_STEPSHUTTER_STEP = 0,	/*	刻み	*/
	PDC_STEPSHUTTER_MIN,		/*	最小値	*/
	PDC_STEPSHUTTER_MAX,		/*	最大値	*/
};

#define	PDC_MUTEX_TIMEOUT		20000
#define	PDC_MUTEX_TIMEOUT_LONG	30000

#endif	/*	_PDC_SUB_FUNC_H_	*/

