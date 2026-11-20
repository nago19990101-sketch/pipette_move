#pragma once
#include "get_create_image.h"

int px = 20;
int py = 20;

Mat input_image = imread("offset.png", IMREAD_ANYCOLOR);
double qua(double a, double b, double c, int s) {
	double sol = 0;
	double d;
	d = b * b - 4 * a * c;
	if (d > 0) {
		if (s == 0) sol = (-b + sqrt(d)) / 2 / a;
		else if (s == 1) sol = (-b - sqrt(d)) / 2 / a;
	}
	else {
		printf("error_qua");
		exit(0);
	}
	return sol;
}

double dsin(double deg) {
	double sol;
	sol = sin(deg / 180. * PI);
	return sol;
}

double dcos(double deg) {
	double sol;
	sol = cos(deg / 180. * PI);
	return sol;
}

double dtan(double deg) {
	double sol;
	sol = tan(deg / 180. * PI);
	return sol;
}

double derive_xangle(double xangle) {
	double a, b, c;
	double x_max;
	double s;

	a = DoF_b * DoF_b + (IMG_WIDTH / 2 - 0.5-px) * pixelpitch / magnification * (IMG_WIDTH / 2 - 0.5-px) * pixelpitch / magnification;
	b = 2 * (IMG_WIDTH / 2 - 0.5-px) * pixelpitch / magnification * D_ob / 2;
	c = -(DoF_b * DoF_b) + D_ob * D_ob / 4;
	double xangle_th = asin(qua(a, b, c, 0)) * 180. / PI;
	//printf("%f\n", xangle_th);
	if (xangle < xangle_th) {
		x_max = (l2 + l3) * 1000 * dtan(xangle) + (IMG_WIDTH / 2 - 0.5-px) * pixelpitch / magnification * dcos(xangle) - D_ob / 2 * dsin(xangle);
		xangle = atan((x_max + (IMG_WIDTH / 2 - 0.5-px) * pixelpitch / magnification * dcos(xangle) - D_ob * dsin(xangle)) / ((l2 + l3) * 1000)) * 180 / PI;
		s = -D_ob / 2 / dsin(xangle) + DoF_f / dtan(xangle);
		if (s < (IMG_WIDTH / 2 - 0.5-px) * pixelpitch / magnification)
			xangle = atan((x_max + s * dcos(xangle) - D_ob * dsin(xangle)) / ((l2 + l3) * 1000)) * 180 / PI;
	}

	else {
		s = -D_ob / 2 / dsin(xangle) - DoF_b / dtan(xangle);
		x_max = (l2 + l3) * 1000 * dtan(xangle) + s * dcos(xangle) - D_ob * dsin(xangle);
		s = -D_ob / 2 / dsin(xangle) + DoF_f / dtan(xangle);
		xangle = atan((x_max + s * dcos(xangle) - D_ob * dsin(xangle)) / ((l2 + l3) * 1000)) * 180 / PI;
		s = -D_ob / 2 / dsin(xangle) + DoF_f / dtan(xangle);
		xangle = atan((x_max + s * dcos(xangle) - D_ob * dsin(xangle)) / ((l2 + l3) * 1000)) * 180 / PI;
	}


	return xangle;
}

double derive_yangle(double yangle) {
	double a, b, c;
	double y_max;
	double s;
	a = DoF_b * DoF_b + (IMG_HEIGHT / 2 - 0.5-py) * pixelpitch / magnification * (IMG_HEIGHT / 2 - 0.5-py) * pixelpitch / magnification;
	b = 2 * (IMG_HEIGHT / 2 - 0.5-py) * pixelpitch / magnification * D_ob / 2;
	c = -(DoF_b * DoF_b) + D_ob * D_ob / 4;
	double yangle_th = asin(qua(a, b, c, 0)) * 180. / PI;

	if (yangle < yangle_th) {
		y_max = l3 * 1000 * dtan(yangle) + (IMG_HEIGHT / 2 - 0.5-py) * pixelpitch / magnification * dcos(yangle) - D_ob / 2 * dsin(yangle);
		yangle = atan((y_max + (IMG_HEIGHT / 2 - 0.5-py) * pixelpitch / magnification * dcos(yangle) - D_ob * dsin(yangle)) / (l3 * 1000)) * 180 / PI;
		s = -D_ob / 2 / dsin(yangle) + DoF_f / dtan(yangle);
		if (s < (IMG_HEIGHT / 2 - 0.5-py) * pixelpitch / magnification)
			yangle = atan((y_max + s * dcos(yangle) - D_ob * dsin(yangle)) / (l3 * 1000)) * 180 / PI;
	}
	else {
		s = -D_ob / 2 / dsin(yangle) - DoF_b / dtan(yangle);
		y_max = l3 * 1000 * dtan(yangle) + s * dcos(yangle) - D_ob * dsin(yangle);
		s = -D_ob / 2 / dsin(yangle) + DoF_f / dtan(yangle);
		yangle = atan((y_max + s * dcos(yangle) - D_ob * dsin(yangle)) / (l3 * 1000)) * 180 / PI;
		s = -D_ob / 2 / dsin(yangle) + DoF_f / dtan(yangle);
		yangle = atan((y_max + s * dcos(yangle) - D_ob * dsin(yangle)) / (l3 * 1000)) * 180 / PI;

	}

	return yangle;
}

double derive_WD(double xangle, double yangle) {
	double WD1;
	WD1 = l1 + l2 / dcos(xangle) + l3 * sqrt(1 + dtan(xangle) * dtan(xangle) + dtan(yangle) * dtan(yangle));
	return WD1;
}

void derive_yangle2(double* xangle, double* yangle, double WD) {

	double WD2 = WD - l1;
	*xangle = 0.0;
	*yangle = acos(l3 / (WD2 - l2)) * 180. / PI;

}

void derive_angle(double gamma, double* xangle, double* yangle, double WD) {

	double a, b, c;
	double WD2 = WD - l1;
	a = (l3 * l3 * (1. + gamma * gamma) - l2 * l2 * (1. - gamma * gamma) + 2. * l2 * l3 * gamma * gamma) * (l3 * l3 * (1. + gamma * gamma) - l2 * l2 * (1. - gamma * gamma) + 2. * l2 * l3 * gamma * gamma);
	b = 2. * (l3 * l3 * (1. + gamma * gamma) - l2 * l2 * (1. - gamma * gamma) + 2. * l2 * l3 * gamma * gamma) * (l3 * l3 - l2 * l2 - WD2 * WD2) - 4. * l2 * l2 * WD2 * WD2;
	c = (l3 * l3 - l2 * l2 - WD2 * WD2) * (l3 * l3 - l2 * l2 - WD2 * WD2) - 4. * l2 * l2 * WD2 * WD2;
	double tanxangle = sqrt(qua(a, b, c, 1));
	*xangle = atan(tanxangle) * 180. / PI;
	*yangle = atan((l2 / l3 + 1) * tanxangle * gamma) * 180. / PI;

}

int store_image_angle(double* xangle, double* yangle, unsigned char* imageaddress, double* storeangleaddress, unsigned char* storeimageaddress) {
	//static int store_image_angle_count = 0;


	*storeangleaddress = *xangle;
	*(storeangleaddress + 1) = *yangle;
	for (int store_image_angle_i = 0; store_image_angle_i < IMG_HEIGHT; store_image_angle_i++) {
		for (int store_image_angle_j = 0; store_image_angle_j < IMG_WIDTH; store_image_angle_j++) {
			*storeimageaddress = *imageaddress;
			storeimageaddress++;
			imageaddress++;
		}
	}
	///store_image_angle_count++;
	//if (store_image_angle_count > nstoreimage)store_image_angle_count = 0;
	//return store_image_angle_count;
}
void create_img(unsigned char* storeimageaddress, Mat image) {
	int  d;
	double x1, y1, z, xvp, yvp;
	double x, y;
	int xpixel, ypixel;
	//int t_x, t_y[IMG_WIDTH], t_d, diff_x, diff_y;
	unsigned char* p_image = image.data;
	//unsigned char* p_Depth = Depth.data;

	unsigned char* storeimageaddress2 = storeimageaddress;

	for (int create_image_i = 0; create_image_i < IMG_HEIGHT; create_image_i++) {
		y1 = -(create_image_i - IMG_HEIGHT / 2 + 0.5) *pixelpitch / magnification;

		for (int create_image_j = 0; create_image_j < IMG_WIDTH; create_image_j++) {

			x1 = (create_image_j - IMG_WIDTH / 2 + 0.5) *pixelpitch / magnification;
			x = x1;
			y = y1;
			xpixel = image.cols / 2 + x;
			ypixel = image.rows / 2 + y;

			//cout << create_image_j << endl;
			//cout << xpixel << "," << ypixel << endl;
			//p_image[(ypixel)*image.cols + (xpixel)] = *storeimageaddress2;
			p_image[(ypixel)*image.cols + (xpixel)] = *storeimageaddress2;


			storeimageaddress2++;
		}
	}
}

void create_image(double z_in, double* storeangleaddress, unsigned char* storeimageaddress, Mat image, Mat Depth, double ddWD, double resolution) {
	int  d;
	double x1, y1, z,xvp,yvp;
	double x, y;
	int xpixel, ypixel;
	//int t_x, t_y[IMG_WIDTH], t_d, diff_x, diff_y;
	unsigned char* p_image = image.data;
	//unsigned char* p_Depth = Depth.data;
	double tanxangle, tanyangle;
	double cosxangle, cosyangle;
	double sinxangle, sinyangle;
	unsigned char* storeimageaddress2 = storeimageaddress;
	tanxangle = -dtan(*storeangleaddress);
	tanyangle = dtan(*(storeangleaddress + 1));
	cosxangle = dcos(*storeangleaddress);
	cosyangle = dcos(*(storeangleaddress + 1));
	sinxangle = -dsin(*storeangleaddress);
	sinyangle = dsin(*(storeangleaddress + 1));
	xvp = (l2 + l3) * 1000 * tanxangle;
	yvp = l3 * 1000 * tanyangle;
	for (int create_image_i = 0; create_image_i < IMG_HEIGHT; create_image_i++) {
	//for (int create_image_i = 0; create_image_i < IMG_HEIGHT; create_image_i++) {
		//y1 = -(create_image_i - IMG_HEIGHT / 2 + 0.5) * pixelpitch / magnification;
		y1 = -(create_image_i - IMG_HEIGHT / 2 +0.5) * pixelpitch / magnification;
		for (int create_image_j = 0; create_image_j < IMG_WIDTH; create_image_j++) {
		//for (int create_image_j = 0; create_image_j < IMG_WIDTH; create_image_j++) {
			//x1 = (create_image_j - IMG_WIDTH / 2 + 0.5) * pixelpitch / magnification;
			x1 = (create_image_j - IMG_WIDTH / 2 + 0.5) * pixelpitch / magnification;
			z = z_in + x1 * sinxangle - y1 * sinyangle;
			d = z * sqrt(1 + tanxangle * tanxangle + tanyangle * tanyangle) - ddWD;
			//if (d > 127) d = 127;
			//else if (d < -128) d = -128;
			//t_x = x;
			x = (xvp + x1 * cosxangle + z * tanxangle) / resolution;
			//diff_x = x - t_x;
			//if (create_image_j == 0) diff_x = 0;
			y = (yvp + y1 * cosyangle - z * tanyangle) / resolution;
			//diff_y = y - t_y[create_image_j];
			//if (create_image_i == 0) diff_y = 0;
			//t_y[create_image_j] = y;

			xpixel = image.cols / 2 + x;
			ypixel = image.rows / 2 + y;
			

			/*
			if (diff_y > 1 && diff_x > 1) {
				for (int com_j = 1; com_j < diff_y + 1; com_j++) {
					for (int com_i = 1; com_i < diff_x + 1; com_i++) {
						t_d = p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] - 128;
						if (p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] == 0) {
							if (com_j < diff_y && com_i < diff_x) {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *(storeimageaddress2 - IMG_WIDTH - 1) / 4 + *storeimageaddress2 / 4 + *(storeimageaddress2 - IMG_WIDTH) / 4 + *(storeimageaddress2 - 1) / 4;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
							else if (com_j < diff_y && com_i >= diff_x) {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - IMG_WIDTH) / 2;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
							else if (com_j >= diff_y && com_i < diff_x) {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - 1) / 2;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
							else {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
						}

						else if (abs(t_d) + 1 >= abs(d)) {
							if (com_j < diff_y && com_i < diff_x) {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *(storeimageaddress2 - IMG_WIDTH - 1) / 4 + *storeimageaddress2 / 4 + *(storeimageaddress2 - IMG_WIDTH) / 4 + *(storeimageaddress2 - 1) / 4;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
							else if (com_j < diff_y && com_i >= diff_x) {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - IMG_WIDTH) / 2;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
							else if (com_j >= diff_y && com_i < diff_x) {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - 1) / 2;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
							else {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
						}
					}
				}
			}

			if (diff_y < -1 && diff_x < -1) {
				for (int com_j = -1; com_j > diff_y - 1; com_j--) {
					for (int com_i = -1; com_i > diff_x - 1; com_i--) {
						t_d = p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] - 128;
						if (p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] == 0) {
							if (com_j > diff_y&& com_i > diff_x) {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *(storeimageaddress2 - IMG_WIDTH - 1) / 4 + *storeimageaddress2 / 4 + *(storeimageaddress2 - IMG_WIDTH) / 4 + *(storeimageaddress2 - 1) / 4;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
							else if (com_j > diff_y&& com_i <= diff_x) {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - IMG_WIDTH) / 2;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
							else if (com_j <= diff_y && com_i > diff_x) {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - 1) / 2;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
							else {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
						}

						else if (abs(t_d) + 1 >= abs(d)) {
							if (com_j > diff_y&& com_i > diff_x) {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *(storeimageaddress2 - IMG_WIDTH - 1) / 4 + *storeimageaddress2 / 4 + *(storeimageaddress2 - IMG_WIDTH) / 4 + *(storeimageaddress2 - 1) / 4;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
							else if (com_j > diff_y&& com_i <= diff_x) {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - IMG_WIDTH) / 2;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
							else if (com_j <= diff_y && com_i > diff_x) {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - 1) / 2;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
							else {
								p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2;
								p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
							}
						}
					}
				}
			}

			else if (diff_y > 1 && diff_x <= 1 && diff_x >= -1) {
				for (int com_j = 1; com_j < diff_y + 1; com_j++) {
					t_d = p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel)] - 128;
					if (p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel)] == 0) {
						if (com_j < diff_y) {
							p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - IMG_WIDTH) / 2;
							p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = d + 128;
						}
						else if (com_j >= diff_y) {
							p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = *storeimageaddress2;
							p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = d + 128;
						}
					}

					else if (abs(t_d) + 1 >= abs(d)) {
						if (com_j < diff_y) {
							p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - IMG_WIDTH) / 2;
							p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = d + 128;
						}
						else if (com_j >= diff_y) {
							p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = *storeimageaddress2;
							p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = d + 128;
						}
					}
				}
			}

			else if (diff_y < -1 && diff_x <= 1 && diff_x >= -1) {
				for (int com_j = -1; com_j > diff_y - 1; com_j--) {
					t_d = p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel)] - 128;
					if (p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel)] == 0) {
						if (com_j > diff_y) {
							p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - IMG_WIDTH) / 2;
							p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = d + 128;
						}
						else if (com_j <= diff_y) {
							p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = *storeimageaddress2;
							p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = d + 128;
						}
					}

					else if (abs(t_d) + 1 >= abs(d)) {
						if (com_j > diff_y) {
							p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - IMG_WIDTH) / 2;
							p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = d + 128;
						}
						else if (com_j <= diff_y) {
							p_image[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = *storeimageaddress2;
							p_Depth[(ypixel - diff_y + com_j) * image.cols + (xpixel)] = d + 128;
						}
					}
				}
			}

			else if (diff_y >= -1 && diff_y <= 1 && diff_x > 1) {
				for (int com_i = 1; com_i < diff_x + 1; com_i++) {
					t_d = p_Depth[(ypixel) * image.cols + (xpixel - diff_x + com_i)] - 128;
					if (p_Depth[(ypixel) * image.cols + (xpixel - diff_x + com_i)] == 0) {
						if (com_i < diff_x) {
							p_image[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - 1) / 2;
							p_Depth[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
						}
						else if (com_i >= diff_x) {
							p_image[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2;
							p_Depth[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
						}
					}

					else if (abs(t_d) + 1 >= abs(d)) {
						if (com_i < diff_x) {
							p_image[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - 1) / 2;
							p_Depth[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
						}
						else if (com_i >= diff_x) {
							p_image[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2;
							p_Depth[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
						}
					}
				}
			}

			else if (diff_y >= -1 && diff_y <= 1 && diff_x < -1) {
				for (int com_i = -1; com_i > diff_x - 1; com_i--) {
					t_d = p_Depth[(ypixel) * image.cols + (xpixel - diff_x + com_i)] - 128;
					if (p_Depth[(ypixel) * image.cols + (xpixel - diff_x + com_i)] == 0) {
						if (com_i > diff_x) {
							p_image[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - 1) / 2;
							p_Depth[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
						}
						else if (com_i <= diff_x) {
							p_image[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2;
							p_Depth[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
						}
					}

					else if (abs(t_d) + 1 >= abs(d)) {
						if (com_i > diff_x) {
							p_image[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2 / 2 + *(storeimageaddress2 - 1) / 2;
							p_Depth[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
						}
						else if (com_i <= diff_x) {
							p_image[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = *storeimageaddress2;
							p_Depth[(ypixel) * image.cols + (xpixel - diff_x + com_i)] = d + 128;
						}
					}
				}
			}
			*/
			//else {
				//t_d = p_Depth[(ypixel) * image.cols + (xpixel)] - 128;
				//if (p_Depth[(ypixel) * image.cols + (xpixel)] == 0) {
					p_image[(ypixel) * image.cols + (xpixel)] = *storeimageaddress2;
					//p_Depth[(ypixel) * image.cols + (xpixel)] = d + 128;
				//}

				//else if (abs(t_d) + 1 >= abs(d)) {
					//p_image[(ypixel) * image.cols + (xpixel)] = *storeimageaddress2;
					//p_Depth[(ypixel) * image.cols + (xpixel)] = d + 128;
				//}
			//}


			storeimageaddress2++;
		}
	}
}

void contrast2(unsigned char* in_imageaddress, unsigned char* out_imageaddress) {
	uchar max = 0, min = 255;
	int I;
	unsigned char* in_p = in_imageaddress;
	unsigned char* out_p = out_imageaddress;
	for (int contrast_i = 0; contrast_i < IMG_HEIGHT; contrast_i++) {
		for (int contrast_j = 0; contrast_j < IMG_WIDTH; contrast_j++) {
			//I=(int)*p-(int)input_image.data[contrast_i*IMG_WIDTH+contrast_j];
			//if(I<0) I=0;
			//else if(I>255)I=255;
			//*p=(uchar)I;
			if (max < *in_p) max = *in_p;
			if (min > * in_p) min = *in_p;
			in_p++;
		}
	}
	if (min >= max) min = max - 1;
	in_p = in_imageaddress;

	for (int contrast_i = 0; contrast_i < IMG_HEIGHT; contrast_i++) {
		for (int contrast_j = 0; contrast_j < IMG_WIDTH; contrast_j++) {
			//I=255*((int)*p-min)/(max-min);
			//if(I<0) I=0;
			//else if(I>255)I=255;
			//*p=(unsigned char)I;
			*out_p = 255 * (*in_p - min) / (max - min);
			in_p++;
			out_p++;
		}
	}

}

void contrast(unsigned char* in_imageaddress) {
	uchar max = 0, min = 255;
	int I;
	unsigned char* in_p = in_imageaddress;
	for (int contrast_i = 0; contrast_i < IMG_HEIGHT; contrast_i++) {
		for (int contrast_j = 0; contrast_j < IMG_WIDTH; contrast_j++) {
			//I=(int)*p-(int)input_image.data[contrast_i*IMG_WIDTH+contrast_j];
			//if(I<0) I=0;
			//else if(I>255)I=255;
			//*p=(uchar)I;
			if (max < *in_p) max = *in_p;
			if (min > * in_p) min = *in_p;
			in_p++;
		}
	}
	if (min >= max) min = max - 1;
	in_p = in_imageaddress;

	for (int contrast_i = 0; contrast_i < IMG_HEIGHT; contrast_i++) {
		for (int contrast_j = 0; contrast_j < IMG_WIDTH; contrast_j++) {
			//I=255*((int)*p-min)/(max-min);
			//if(I<0) I=0;
			//else if(I>255)I=255;
			//*p=(unsigned char)I;
			*in_p = 255 * (*in_p - min) / (max - min);
			in_p++;
		}
	}

}

int make_angle_table(int X) {
	//X = 30;
	int expand_viewpoint_num = 0;
	double WD_x, WD_y;
	double WD;
	double xangle, yangle;
	xangle = 0;
	yangle = 0;
	double save_WD[30][1000];

	plist = (double**)malloc(sizeof(double*) * X);     /* X個のint型ポインタのためのメモリー領域を確保 */
	//plist_angle = (double**)malloc(sizeof(double*) * X);     /* X個のint型ポインタのためのメモリー領域を確保 */

	for (int i = 0; i < X; i++) {

		/* n個のint型変数のためのメモリー領域を確保し、そのアドレスをポインタ配列plistのi番目の要素として格納 */
		ndata[i] = i * 2 * 4 ;
		//if(i>2) ndata[i] = (i * 2-1) * 4;
		//ndata[i] = (i + 1) * 4;
		ndata[0] = 1;
		expand_viewpoint_num = expand_viewpoint_num + ndata[i];
		plist[i] = (double*)malloc(4 * ndata[i] * sizeof(double));

	}



	for (int i = 0; i < X; i++)          /* 各データ配列の先頭アドレスを表示 */

		printf("address of array #%d: %p\n", i, plist[i]);

	for (int i = 0; i < X; i++) {
		double* p = plist[i];
		double inclination = 0;
		double inclinationx = 0;
		double inclinationy = 0;
		double inclination_thres;
		int k1 = 0;
		int k2 = 0;
		int k3 = 0;
		for (int j = 0; j < ndata[i]; j++) {
			if (j == 0) {
				if (i == 0) {
					xangle = 0.;
					yangle = 0.;
					*p = xangle;
					*(p + 1) = yangle;
					*(p + 2) = (l2 + l3) * -dtan(xangle)*1000;
					*(p + 3) = -l3 * dtan(yangle) * 1000;
					p = p + 4;
				}
				else {
					xangle = derive_xangle(xangle);
					yangle = derive_yangle(yangle);
					WD_x = derive_WD(xangle, 0.);
					WD_y = derive_WD(0., yangle);
					WD = WD_x;
					if (WD_x > WD_y) WD = WD_y;
					//printf("WD=%f",WD);
					derive_yangle2(p, p + 1, WD);
					yangle = *(p + 1);
					derive_angle(0., p, p + 1, WD);
					xangle = *p;

					*(p + 2) = (l2 + l3) * -dtan(*p) * 1000;
					*(p + 3) = l3 * dtan(*(p + 1)) * 1000;

					inclinationx = ((IMG_HEIGHT) / magnification * pixelpitch) / ((l2 + l3) * 1000 * dtan(xangle));
					inclinationy = ((IMG_WIDTH) / magnification * pixelpitch) / (l3 * 1000 * dtan(yangle));
					inclination_thres = (l3 * 1000 * dtan(yangle)) / ((l2 + l3) * 1000 * dtan(xangle));

					p = p + 4;
				}
			}
			else if (j < ndata[i] / 4) {
				if (inclination <= inclination_thres) inclination = inclination + inclinationx;
				if (inclination > inclination_thres) inclination = 1 / ((ndata[i] / 4 - j) * inclinationy);

				derive_angle(inclination, p, p + 1, WD);
				*(p + 2) = (l2 + l3) * -dtan(*p) * 1000;
				*(p + 3) = l3 * dtan(*(p+1)) * 1000;
				p = p + 4;
			}
			else if (j == ndata[i] / 4) {
				derive_yangle2(p, p + 1, WD);
				yangle = *(p + 1);
				*(p + 2) = (l2 + l3) * -dtan(*p) * 1000;
				*(p + 3) = l3 * dtan(*(p + 1)) * 1000;
				p = p + 4;
			}
			else if (ndata[i] / 4 + 1 <= j && j < ndata[i] / 2 + 1) {
				*p = -*(plist[i] + (ndata[i] / 4 - 1 - k1) * 4);
				*(p + 1) = *(plist[i] + (ndata[i] / 4 - 1 - k1) * 4 + 1);
				*(p + 2) = (l2 + l3) * -dtan(*p) * 1000;
				*(p + 3) = l3 * dtan(*(p + 1)) * 1000;
				p = p + 4;
				k1++;
			}
			else if (ndata[i] / 2 + 1 <= j && j < ndata[i] * 3 / 4 + 1) {
				*p = -*(plist[i] + (k2 + 1) * 4);
				*(p + 1) = -*(plist[i] + (k2 + 1) * 4 + 1);
				*(p + 2) = (l2 + l3) * -dtan(*p) * 1000;
				*(p + 3) = l3 * dtan(*(p + 1)) * 1000;
				p = p + 4;
				k2++;
			}
			else if (ndata[i] * 3 / 4 + 1 <= j && j < ndata[i]) {
				*p = *(plist[i] + (ndata[i] / 4 - 1 - k3) * 4);
				*(p + 1) = -*(plist[i] + (ndata[i] / 4 - 1 - k3) * 4 + 1);
				*(p + 2) = (l2 + l3) * -dtan(*p) * 1000;
				*(p + 3) = l3 * dtan(*(p + 1)) * 1000;
				p = p + 4;
				k3++;
			}

		}
	}

	char filename[50];
	//グラフの書き込み
	FILE* fp;
	sprintf(filename, "Amirror_angle.csv");
	fp = fopen(filename, "w");
	if (fp == NULL)
		exit(1);

	fprintf(fp, "count,xangle,yangle,xpos, ypos, WD\n");

	//グラフの値を入力(角度のみ)
	int xpos, ypos;
	for (int i = 0; i < X; i++) {
		double* p = plist[i];
		for (int j = 0; j < ndata[i]; j++) {
			xangle = *p;
			p++;
			yangle = *p;
			p++;
			xpos = *p;
			p++;
			ypos = *p;
			p++;

			save_WD[i][j] = derive_WD(xangle, yangle);
			fprintf(fp, "%d,%f,%f,%d,%d,%f\n", i, xangle, yangle,xpos,ypos, save_WD[i][j]);
		}
	}

	return expand_viewpoint_num;
}

void malloc_for_image() {

	printf("nstoreimage=%d\n", nstoreimage);
	printf("nstorepointedimage=%d\n", nstorepointedimage);
	pangle = (double**)malloc(sizeof(double*) * nstoreimage);     /* X個のint型ポインタのためのメモリー領域を確保 */
	ppointedangle = (double**)malloc(sizeof(double*) * nstorepointedimage);     /* X個のint型ポインタのためのメモリー領域を確保 */
	pimage = (unsigned char**)malloc(sizeof(unsigned char*) * nstoreimage);     /* X個のint型ポインタのためのメモリー領域を確保 */
	ppointedimage = (unsigned char**)malloc(sizeof(unsigned char*) * nstorepointedimage);     /* X個のint型ポインタのためのメモリー領域を確保 */
	
	for (int i = 0; i < nstoreimage; i++) {

		/* n個のint型変数のためのメモリー領域を確保し、そのアドレスをポインタ配列plistのi番目の要素として格納 */

		pangle[i] = (double*)malloc(2 * sizeof(double));
		pimage[i] = (unsigned char*)malloc(IMG_HEIGHT * IMG_WIDTH * sizeof(unsigned char));

	}

	for (int i = 0; i < nstorepointedimage; i++) {

		/* n個のint型変数のためのメモリー領域を確保し、そのアドレスをポインタ配列plistのi番目の要素として格納 */

		ppointedangle[i] = (double*)malloc(2 * sizeof(double));
		ppointedimage[i] = (unsigned char*)malloc(IMG_HEIGHT * IMG_WIDTH * sizeof(unsigned char));

	}

}

void free_pointer(int X) {
	for (int i = 0; i < X; i++) {
		free(plist[i]);
	}
	free(plist);

	for (int i = 0; i < nstoreimage; i++) {
		free(pangle[i]);
		free(pimage[i]);
	}
	free(pangle);
	free(pimage);

	for (int i = 0; i < nstorepointedimage; i++) {
		free(ppointedangle[i]);
		free(ppointedimage[i]);
	}
	free(ppointedangle);
	free(ppointedimage);

}

