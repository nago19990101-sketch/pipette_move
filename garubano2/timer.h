/**
* @file    mytimer.h
* @brief   タイマーライブラリ
* @author  Kensuke FUJIWARA 
* @date	   2006/07/16 - 2007/03/04
* @version 1.0
*/
#include <iostream>
#include <vector>
#include <ctime>

#include "windows.h"

typedef unsigned int uint;


#ifndef _TIMER_H__DDBCBACA_0F00_4236_8433_5AE5BDD0F071
#define _TIMER_H__DDBCBACA_0F00_4236_8433_5AE5BDD0F071

 
/// タイマーライブラリ
namespace mytimer{


/// ログデータ
// ログデータ
struct TimerLog { 
	
	LONGLONG	count,		///< カウント値
				freq;		///< 周波数
};


/// Timer クラス
// Timer クラス
class Timer {

	static const uint MemResize = 100;		///< リサイズ時の増分

protected:

	LARGE_INTEGER begin;		///< カウント開始値
	LARGE_INTEGER end;			///< カウント終了値
	LARGE_INTEGER freq;			///< クロックの周波数
	LONGLONG overhead;			///< 呼び出し時のオーバヘッド

	bool is_logger_on;			///< ログ機能ＯＮ
	std::vector<TimerLog> log;	///< ログデータ
	uint	idx;				///< ログを格納する位置

public:

	Timer();

	/**
	* @brief	デストラクタ
	* @param	なし
	* @return	なし
	*/
	~Timer()
	{
#ifdef _DEBUG
		std::cout << "Timer::~Timer()" << std::endl;
#endif
	};


	// 初期化
	void init(bool is_logger_on=false);
	void init(uint size, bool is_logger_on=false);
	// 計測開始
	void start();
	// 計測終了
	void stop();

	// 経過時間の取得
	double msec();
	double msec(TimerLog tmrlog);
	double usec();
	double usec(TimerLog tmrlog);

	// 経過時間の最小値の取得
	bool getMinTime(TimerLog &min);
	// 経過時間の最大値の取得
	bool getMaxTime(TimerLog &max);
	// 経過時間の平均値の取得
	bool getAveTime(TimerLog &ave);


	// ログデータの経過時間[msec]の表示
	/**
	* @brief		ログデータの経過時間[msec]の一覧表示
	* @param[in]	stream	ストリーム
	* @param[in]	timer	一覧を表示したいタイマークラス
	*/
	friend std::ostream &operator<<(std::ostream &stream, Timer &timer);
};
/**
* @example testTimer.cpp
* @example main.cpp
*/


}

#endif