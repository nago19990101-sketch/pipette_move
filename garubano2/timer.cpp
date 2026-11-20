/**
* @file    mytimer.cpp
* @brief   タイマーライブラリ
* @author  Kensuke FUJIWARA 
* @date	   2006/07/16 - 2007/03/04
* @version 1.0
*/
#include "timer.h"
using namespace std;
using namespace mytimer;

/**
* @brief		コンストラクタ
* @param		なし
* @return		なし
*/
Timer::Timer()
{
#ifdef _DEBUG
		cout << "Timer::Timer()" << endl;
#endif

	is_logger_on = false;

	// パフォーマンスカウンタが存在するかどうか確認
	LARGE_INTEGER temp;
	if(!QueryPerformanceFrequency(&temp)){

		cout <<"Can't find Performance Counter" << endl;
		abort();
	}

	init();
}


/**
* @brief		タイマーの初期化
* @param[in]	is_logger_on ログ機能をＯＮにするかどうか
* @return		なし
*/
void Timer::init(
				 
	bool is_logger_on	// ログ機能をＯＮにするかどうか
	
){
	//start();
	//stop();
	//overhead = (end.QuadPart - begin.QuadPart);
	overhead=0;

	log.clear();

	this->is_logger_on = is_logger_on;
	idx  = 0;
}


/**
* @brief		タイマーの初期化
* @note			sizeを超えた場合には自動的に拡張される．
* @param[in]	size			ログ領域のサイズ
* @param[in]	is_logger_on	ログ機能をＯＮにするかどうか
* @return		なし
*/void Timer::init(
				   
	uint size,				// ログ領域のサイズ
	bool is_logger_on		// ログ機能をＯＮにするかどうか
	
){
	//start();
	//stop();
	//overhead = (end.QuadPart - begin.QuadPart);
	overhead=0;

	log.clear();
	log.resize(size);

	this->is_logger_on = is_logger_on;
	idx  = 0;
}




/**
* @brief		タイマースタート
* @param		なし
* @return		なし
*/
void Timer::start()
{
	QueryPerformanceCounter(&begin);
}



/**
* @brief		タイマーストップ
* @param		なし
* @return		なし
*/
void Timer::stop()
{
	TimerLog temp;
	QueryPerformanceFrequency(&freq);
 	QueryPerformanceCounter(&end);

	temp.count = (end.QuadPart - begin.QuadPart >= overhead)? end.QuadPart - begin.QuadPart - overhead : 0;
	temp.freq  = freq.QuadPart;

	if(is_logger_on){
		
		if(idx >= log.size()){
			
			cout << "Resize : " << log.size() + MemResize << endl;
			log.resize(log.size() + MemResize);
		}

		log[idx] = temp;
		idx++;
	}
}


/**
* @brief		経過時間の取得[msec]
* @param		なし
* @return		経過時間
*/
double Timer::msec()
{
	return ((double)(end.QuadPart - begin.QuadPart - overhead) * 1000.0) / freq.QuadPart;
}


/**
* @brief		経過時間の取得[msec]
* @note			カウント値を経過時間に変換する
* @param[in]	tmrlog ログ
* @return		経過時間
*/
double Timer::msec(
				   
	TimerLog tmrlog		// ログ
	
){
	return ((double)tmrlog.count / tmrlog.freq) * 1000.0;
}


/**
* @brief		経過時間の取得[usec]
* @param		なし
* @return		経過時間
*/
double Timer::usec()
{
	return ((double)(end.QuadPart - begin.QuadPart - overhead) * 1000000.0) / freq.QuadPart;
}


/**
* @brief		経過時間の取得[usec]
* @note			カウント値を経過時間に変換する
* @param[in]	tmrlog ログ
* @return		経過時間
*/
double Timer::usec(
				   
	TimerLog tmrlog		// ログ
	
){
	return ((double)tmrlog.count * 1000000.0) / tmrlog.freq;
}



/**
* @brief		経過時間の最小値を取得
* @param[out]	min 経過時間の最小値
* @return		成功：true \n
*				失敗：false（経過時間の最小値が計算できない）
*/
bool Timer::getMinTime(
					   
	TimerLog &min		// 経過時間の最小値
	
){
	if(is_logger_on && (log.size() > 0)){

		min.count = log[0].count;
		for(uint i=1; i<idx; i++){

			if(min.count > log[i].count){

				min.count = log[i].count;
				min.freq  = log[i].freq;
			}
		}

		return true;
	}

	return false;
}


/**
* @brief		経過時間の最大値を取得
* @param[out]	max 経過時間の最大値
* @return		成功：true \n
*				失敗：false（経過時間の最大値が計算できない）
*/
bool Timer::getMaxTime(
						   
	TimerLog &max		// 経過時間の最大値
	
){
	if(is_logger_on && (log.size() > 0)){

		max.count = log[0].count;
		for(uint i=1; i<idx; i++){

			if(max.count < log[i].count){

				max.count = log[i].count;
				max.freq  = log[i].freq;
			}
		}

		return true;
	}

	return false;
}



/**
* @brief		経過時間の平均値を取得
* @param[out]	ave 経過時間の平均値
* @return		成功：true \n
*				失敗：false（経過時間の平均値が計算できない）
*/
bool Timer::getAveTime(
					   
	TimerLog &ave		// 経過時間の平均値
	
){
	if(is_logger_on && (log.size() > 0)){

	
		ave.count = ave.freq = 0;
		for(uint i=0; i<idx; i++){

			ave.count += log[i].count;
			ave.freq  += log[i].freq;
		}

		ave.count = (LONGLONG)((double)ave.count / idx);
		ave.freq  = (LONGLONG)((double)ave.freq  / idx);

		return true;
	}

	return false;
}


/**
* @brief		ログデータの経過時間[msec]の一覧表示
* @param[in]	stream	ストリーム
* @param[in]	timer	一覧を表示したいタイマークラス
*/
ostream &mytimer::operator<<(
							 
	ostream &stream,	// ストリーム 
	Timer	&timer		// 一覧を表示したいタイマークラス
	
){
	for(uint i=0; i<timer.idx; i++){

		stream << timer.msec(timer.log[i]) << endl;
	}

	stream << "num : " << (uint)timer.log.size() << endl;

	return stream;
}
