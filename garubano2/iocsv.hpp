////////////////////////////////////////////////////////////
/*!
	@file 
	@brief CSVファイル入出力用クラス．
	ネットで発見したものを実数でも使用できるよう修正した．
*/
////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <string>
using namespace std;

#ifndef __IOCSV_HPP_INCLUDED__
#define __IOCSV_HPP_INCLUDED__

//! 整数，実数のCSV入出力クラス
class CistreamCsv {
    //! マニピュレータ endl．行末までの文字を読み飛ばす．
    friend CistreamCsv& endl(CistreamCsv& riscsv) {
        int iChar = 0;
        while ((iChar = riscsv.m_ris.get()) != '\n' && iChar != EOF) {}        // 行末まで読み飛ばす。
        return riscsv;
    }
private:
	//! 入力に使う istream の参照
    istream& m_ris;
public:
    CistreamCsv(istream& ris) : m_ris(ris) {}
	/*! @brief 整数の読込用演算子
	@param[out] ri 格納用バッファ
	@return 読み込んだCSVファイルのCistreamCSV
	*/
    CistreamCsv& operator >>(int& ri) {
        ri = 0;
        char iChar = 0;
        // 空白を読み飛ばす。
        while (isspace(iChar = m_ris.get()) && iChar != '\n') {
		}
        if (iChar == EOF ) { 
			return *this; 
		}
        string strValue;
        if (iChar == '\n') { 
			m_ris.putback(iChar); iChar = 0; 
		} else { 
			strValue += iChar;               
		}
        // コンマまで読む。
        while ((iChar = m_ris.get()) != ',' && iChar != EOF && iChar != '\n') {
            strValue += iChar;
        }
        ri = atoi(strValue.c_str());
        if (iChar == EOF ) { 
			return *this;         
		}
        if (iChar == '\n') { 
			m_ris.putback(iChar); 
		}
        return *this;
    }

	 CistreamCsv& operator >>(long& ri) {
        ri = 0;
        char iChar = 0;
        // 空白を読み飛ばす。
        while (isspace(iChar = m_ris.get()) && iChar != '\n') {
		}
        if (iChar == EOF ) { 
			return *this; 
		}
        string strValue;
        if (iChar == '\n') { 
			m_ris.putback(iChar); iChar = 0; 
		} else { 
			strValue += iChar;               
		}
        // コンマまで読む。
        while ((iChar = m_ris.get()) != ',' && iChar != EOF && iChar != '\n') {
            strValue += iChar;
        }
		ri = atol(strValue.c_str());
        if (iChar == EOF ) { 
			return *this;         
		}
        if (iChar == '\n') { 
			m_ris.putback(iChar); 
		}
        return *this;
    }
	
	/*! @brief 実数の読込用演算子
	@param[out] ri 格納用バッファ
	@return 読み込んだCSVファイルのCistreamCSV
	*/
	CistreamCsv& operator >>(double& ri) {
        ri = 0;
        int iChar = 0;
        // 空白を読み飛ばす。
        while (isspace(iChar = m_ris.get()) && iChar != '\n') {}
        if (iChar == EOF ) { return *this; }
        string strValue;
        if (iChar == '\n') { m_ris.putback(iChar); iChar = 0; }
        else               { strValue += iChar;               }
        // コンマまで読む。
        while ((iChar = m_ris.get()) != ',' && iChar != EOF && iChar != '\n') {
            strValue += iChar;
        }
        ri = atof(strValue.c_str());
        if (iChar == EOF ) { return *this;         }
        if (iChar == '\n') { m_ris.putback(iChar); }
        return *this;
    }

    //! マニピュレータを受け入れるための挿入演算子
    CistreamCsv& operator >>(CistreamCsv& (*pmanipulatorfunction)(CistreamCsv&)) {
        return (*pmanipulatorfunction)(*this);
    }
};

#endif