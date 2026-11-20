#pragma once
// 通常視野 circle_img 上のクリック位置 → マニピュレータ座標 の変換（Z固定）
//
// 正変換（既存）:
//   man*_pos3D = (xw, -yw, zw)
//   projectPoints(man*_pos3D, rvec, tvec, K, distortion, man*_pos2D)
//   man*_cpos  = man*_pos2D + (man*_dx, man*_dy)
// この関数はその逆変換で、ワールド座標の z が zw になる点を光線上で求める。

#include "opencv2/core.hpp"

namespace click_move {

	// クリック画素 (u, v) から、z = zw の平面上のマニピュレータ座標 (xw, yw) を求める。
	// tip_dx, tip_dy : 先端補正 man*_dx, man*_dy（正変換で足している値）
	// 戻り値 false : 光線が z = zw 平面とほぼ平行で解けない
	bool pixelToWorldFixedZ(double u, double v, double zw,
		const cv::Mat& rvec, const cv::Mat& tvec, const cv::Mat& K,
		double tip_dx, double tip_dy,
		double& xw, double& yw);

	// (xw, yw, zw) を正変換で画素に戻し、(u, v) とのずれ [px] を返す（検算用）
	double reprojectError(double u, double v, double xw, double yw, double zw,
		const cv::Mat& rvec, const cv::Mat& tvec, const cv::Mat& K,
		double tip_dx, double tip_dy);

}
