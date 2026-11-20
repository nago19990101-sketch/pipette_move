#include <windows.h>
#include "opencv2/opencv.hpp"
#include <sstream>
using namespace cv;
using namespace std;

#define PIXEL(image, x, y)     (((unsigned char*)(image)->imageData + (y)*(image)->widthStep)[(x)])

#define PIXEL_B(image, x, y)   (((unsigned char*)(image)->imageData + (y)*(image)->widthStep + (x)*(image)->nChannels)[0])
#define PIXEL_G(image, x, y)   (((unsigned char*)(image)->imageData + (y)*(image)->widthStep + (x)*(image)->nChannels)[1])
#define PIXEL_R(image, x, y)   (((unsigned char*)(image)->imageData + (y)*(image)->widthStep + (x)*(image)->nChannels)[2])

#define PIXEL_H(image, x, y)   PIXEL_B(image, x, y)
#define PIXEL_S(image, x, y)   PIXEL_G(image, x, y)
#define PIXEL_V(image, x, y)   PIXEL_R(image, x, y)

typedef struct _Moment
{
    int M0, Mx, My;
} MOMENT;

typedef struct _HSVThreshold
{
    int UpperHue, LowerHue, Saturation, Value;
} HSVTHRESHOLD;

int HSVBinarization(Mat & imgHSV, Mat & imgBin, HSVTHRESHOLD hsvth);

int TotalMoment(Mat & imgBin, MOMENT *mmt);

int PlotGravity(Mat & img, MOMENT & mmt, Scalar sclColor = Scalar(0,0,255));
int otsu(const unsigned int *ihist);