#pragma once
#include "output_message.h"
#include <string>


//ミラーが制限範囲外になるときにメッセージを出力
void warn_mirror_range(double mirror_x, double mirror_y) {
	//範囲外にあるとき
	if (abs(mirror_x) > 10 || abs(mirror_y) > 10) {
		if (mirror_x > 10) {
			mirror_x = 10;
			printf("mirror_x_over_10");
		}
		else if (mirror_x < -10) {
			mirror_x = -10;
			printf("mirror_x_over_-10");
		}
		if (mirror_y > 10) {
			mirror_y = 10;
			printf("mirror_y_over_10");
		}
		else if (mirror_y < -10) {
			mirror_y = -10;
			printf("mirror_y_over_-10");
		}
	}


}
