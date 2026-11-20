#include "click_move.h"

#include <cmath>
#include <vector>
#include "opencv2/calib3d.hpp"

namespace click_move {

	bool pixelToWorldFixedZ(double u, double v, double zw,
		const cv::Mat& rvec, const cv::Mat& tvec, const cv::Mat& K,
		double tip_dx, double tip_dy,
		double& xw, double& yw)
	{
		if (rvec.empty() || tvec.empty() || K.empty()) return false;

		cv::Mat R;
		cv::Rodrigues(rvec, R);
		cv::Matx33d Rm = R;
		cv::Vec3d t(tvec.at<double>(0, 0), tvec.at<double>(1, 0), tvec.at<double>(2, 0));

		const double fx = K.at<double>(0, 0);
		const double fy = K.at<double>(1, 1);
		const double cx = K.at<double>(0, 2);
		const double cy = K.at<double>(1, 2);

		// カメラ座標系の光線方向（歪みは distortion = 0 なので考えない）
		cv::Vec3d d((u - tip_dx - cx) / fx, (v - tip_dy - cy) / fy, 1.0);

		// カメラ座標 Pc = λd、ワールド座標 P = Rᵀ(Pc - t) = λa - b
		cv::Vec3d a = Rm.t() * d;
		cv::Vec3d b = Rm.t() * t;

		// man*_pos3D の z 成分は zw そのもの（符号反転なし）
		if (std::fabs(a[2]) < 1e-12) return false;
		const double lambda = (zw + b[2]) / a[2];
		cv::Vec3d P = lambda * a - b;

		xw = P[0];
		yw = -P[1];	// man*_pos3D は y を反転しているので元に戻す
		return true;
	}

	double reprojectError(double u, double v, double xw, double yw, double zw,
		const cv::Mat& rvec, const cv::Mat& tvec, const cv::Mat& K,
		double tip_dx, double tip_dy)
	{
		std::vector<cv::Point3d> obj{ cv::Point3d(xw, -yw, zw) };
		std::vector<cv::Point2d> img;
		cv::Mat dist = cv::Mat::zeros(4, 1, CV_64F);
		cv::projectPoints(obj, rvec, tvec, K, dist, img);
		const double du = img[0].x + tip_dx - u;
		const double dv = img[0].y + tip_dy - v;
		return std::sqrt(du * du + dv * dv);
	}

}
