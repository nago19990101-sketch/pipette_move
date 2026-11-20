#include "OpenCVConfig.h"

int HSVBinarization(Mat & imgHSV, Mat & imgBin, HSVTHRESHOLD hsvth)
{
    unsigned char *imgHSVimageData = imgHSV.data;
    unsigned char *imgBinimageData = imgBin.data;
    int U_H, L_H;
    int Hue, Saturation, Value;

	//rectangle(imgBin, Rect(0,0,imgBin.cols,imgBin.rows), Scalar(0,0,0), CV_FILLED);
   
        for(int y = 0; y < imgHSV.rows; y++, imgHSVimageData += imgHSV.step, imgBinimageData += imgBin.step){        
            for(int x = 0; x < imgHSV.cols; x++){
                Hue         = imgHSVimageData[x*3 + 0];
                Saturation  = imgHSVimageData[x*3 + 1];
                Value       = imgHSVimageData[x*3 + 2];
                if(!(Saturation > hsvth.Saturation)) continue;
                if(!(Value > hsvth.Value)) continue;
				if(hsvth.UpperHue > hsvth.LowerHue) {
					if(!(Hue <= hsvth.UpperHue)) continue;
					if(!(Hue > hsvth.LowerHue)) continue;
					imgBinimageData[x] = 0xff;
				}else{
					U_H = (Hue <= hsvth.UpperHue) ? 0xff : 0x00;
					L_H = (hsvth.LowerHue < Hue) ? 0xff : 0x00;
					imgBinimageData[x] = (unsigned char)(U_H | L_H);
				}
            }
        }

    return 0;
}

int TotalMoment(Mat & imgBin, MOMENT *mmt)
{
	memset(mmt, 0, sizeof(MOMENT));
    unsigned char *imgBinimageData = imgBin.data;
    for(int y = 0; y < imgBin.rows; y++, imgBinimageData += imgBin.step) {
        for(int x = 0; x < imgBin.cols; x++) {
            if(imgBinimageData[x] == 0) continue;
            mmt->M0++;
            mmt->Mx += x;
            mmt->My += y;
        }
    }
    return 0;
}

int PlotGravity(Mat & img, MOMENT & mmt, Scalar sclColor)
{
    int x = (int)((double)mmt.Mx/mmt.M0 + 0.5);
    int y = (int)((double)mmt.My/mmt.M0 + 0.5);
    line(img, Point(x - 10, y - 10), Point(x + 10, y + 10), sclColor, 4);
    line(img, Point(x - 10, y + 10), Point(x + 10, y - 10), sclColor, 4);
    return 0;
}

int otsu(const unsigned int *ihist) {

	int thresholdValue = 1;

	int k; // various counters
	int n, n1, n2, gmin, gmax;
	double m1, m2, sum, csum, fmax, sb;

	gmin = 255;
	gmax = 0;

	// set up everything
	sum = csum = 0.0;
	n = 0;

	for (k = 0; k <= 255; k++) {
		sum += (double) k * (double) ihist[k]; /* x*f(x)*/
		n += ihist[k]; /*  f(x) */
	}

	// do the otsu global thresholding method
	fmax = -1.0;
	n1 = 0;
	for (k = 0; k < 255; k++) {
		n1 += ihist[k];
		if (!n1) {
			continue;
		}
		n2 = n - n1;
		if (n2 == 0) {
			break;
		}
		csum += (double) k * ihist[k];
		m1 = csum / n1;
		m2 = (sum - csum) / n2;
		sb = (double) n1 * (double) n2 * (m1 - m2) * (m1 - m2);
		/* bbg: note: can be optimized. */
		if (sb > fmax) {
			fmax = sb;
			thresholdValue = k;
		}
	}
	return thresholdValue;
}
