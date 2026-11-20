#define PI				3.14159265358979		// πの定義
#include <sstream>
#include "math.h"

//操作点3点：スプライン曲線計算用，初期の傾きはゼロ，終端の傾きはゼロ
double Curve1(double x_0, double y_0, double x_1, double y_1, double ratio, double x){ //dy_0=dy_1=0
	int n=3;
	double p[3],q[3],r[3],s[3];
	double xdata[3],ydata[3];
	xdata[0]=x_0;
	xdata[1]=ratio*x_1+(1.0-ratio)*x_0;
	xdata[2]=x_1;
	
	ydata[0]=y_0;
	ydata[1]=(y_1+y_0)/2.0;
	ydata[2]=y_1;

	double h_0=xdata[1]-xdata[0];
	double h_1=xdata[2]-xdata[1];
	
	r[1]=3.0*(ydata[2]-ydata[1])/(h_1*(h_1+h_0))-3.0*(ydata[1]-ydata[0])/(h_0*(h_1+h_0));
	s[0]=r[1]/(2.0*h_0)-(ydata[1]-ydata[0])/(2.0*h_0*h_0*h_0);
	s[1]=-r[1]/(2.0*h_1)-(ydata[1]-ydata[0])/(2.0*h_1*h_1*h_1);
	r[0]=r[1]-3.0*s[0]*h_0;
	q[1]=-2.0*r[1]*h_1-3.0*s[1]*h_1*h_1;
	q[0]=0.0;
	p[0]=ydata[0];
	p[1]=ydata[1];

	int fg;
	double z;
	fg=0;
	for(int i=0; i<n-1; i++){
		if((x<xdata[i+1])&&(fg==0)){
			fg=1;
			double zz=x-xdata[i];
			z=p[i]+zz*(q[i]+zz*(r[i]+zz*s[i]));
		}
	}

	if(x>=x_1) z=y_1;
	if(x<=x_0) z=y_0;
	
	return z;
}


//操作点3点：スプライン曲線計算用，初期の傾きはゼロ，終端の傾きは開放
double Curve2(double x_0, double y_0, double x_1, double y_1, double ratio, double x){
	int n=3;
	double p[3],q[3],r[3],s[3];
	double xdata[3],ydata[3];
	if(x < x_0) x=x_0;
	else if(x > x_1) x=x_1;

	xdata[0]=x_0;
	xdata[1]=ratio*x_1+(1.0-ratio)*x_0;
	xdata[2]=x_1;
	
	ydata[0]=y_0;
	ydata[1]=(y_1+y_0)/2.0;
	ydata[2]=y_1;

	double h_0=xdata[1]-xdata[0];
	double h_1=xdata[2]-xdata[1];
	
	s[0]=(3.0*(ydata[2]-ydata[1])/h_1-(6.0*h_0+2.0*h_1)*(ydata[1]-ydata[0])/(h_0*h_0))/(3.0*h_0*h_0+4.0*h_0*h_1);
	r[0]=-s[0]*h_0+(ydata[1]-ydata[0])/(h_0*h_0);
	r[1]=r[0]+3.0*s[0]*h_0;
	s[1]=-r[1]/(3.0*h_1);
	q[1]=2.0*r[0]*h_0+3.0*s[0]*h_0*h_0;
	q[0]=0.0;
	p[0]=ydata[0];
	p[1]=ydata[1];

	int fg;
	double z;
	fg=0;
	for(int i=0; i<n-1; i++){
		if((x<xdata[i+1])&&(fg==0)){
			fg=1;
			double zz=x-xdata[i];
			z=p[i]+zz*(q[i]+zz*(r[i]+zz*s[i]));
		}
	}
	return z;
}

double SIN(double ang){
	double value=sin(ang*PI/180.0);
	return value;
}

double COS(double ang){
	double value=cos(ang*PI/180.0);
	return value;
}

double TAN(double ang){
	double value=tan(ang*PI/180.0);
	return value;
}

double ASIN(double P){
	if(P>1.0){
		P=1.0;
	}else if(P<-1.0){
		P=-1.0;
	}
	double ang=asin(P)*180.0/PI;
	return ang;
}

double ACOS(double P){
	if(P>1.0){
		P=1.0;
	}else if(P<-1.0){
		P=-1.0;
	}
	double ang=acos(P)*180.0/PI;
	return ang;
}

double ATAN(double P){
	double ang=atan(P)*180.0/PI;
	return ang;
}

double Sqrt(double P){
	if(P<0.0){
		P=0.0;
	}
	double root=sqrt(P);
	return root;
}

double ASINh(double P){
	double Q1=P+sqrt(P*P+1);
	double ans=log(Q1);
	return ans;
}

double ACOSh(double P){
	double Q1=P+sqrt(P*P-1);
	double ans=log(Q1);
	return ans;
}
double AddASIN(double a, double b, double c){
	double ans;
	if(a==0){
		ans=ACOS(c/b);
	}else if(a<0){
		a=-a;
		b=-b;
		c=-c;
		double Q1=c/sqrt(a*a+b*b);
		ans=ASIN(Q1)-ATAN(b/a);
	}else{
		double Q1=c/sqrt(a*a+b*b);
		ans=ASIN(Q1)-ATAN(b/a);
	}
	
	return ans;
}
double AddACOS(double a, double b, double c){
	double ans;
	if(a==0){
		ans=ASIN(c/b);
	}else if(a<0){
		a=-a;
		b=-b;
		c=-c;
		double Q1=c/sqrt(a*a+b*b);
		ans=ACOS(Q1)+ATAN(b/a);
	}else{
		double Q1=c/sqrt(a*a+b*b);
		ans=ACOS(Q1)+ATAN(b/a);
	}
	
	return ans;
}

double Addasin(double a, double b, double c){
	double ans;
	if(a==0){
		ans=acos(c/b);
	}else if(a<0){
		a=-a;
		b=-b;
		c=-c;
		double Q1=c/sqrt(a*a+b*b);
		ans=asin(Q1)-atan(b/a);
	}else{
		double Q1=c/sqrt(a*a+b*b);
		ans=asin(Q1)-atan(b/a);
	}
	
	return ans;
}
double Addacos(double a, double b, double c){
	double ans;
	if(a==0){
		ans=asin(c/b);
	}else if(a<0){
		a=-a;
		b=-b;
		c=-c;
		double Q1=c/sqrt(a*a+b*b);
		ans=acos(Q1)+atan(b/a);
	}else{
		double Q1=c/sqrt(a*a+b*b);
		ans=acos(Q1)+atan(b/a);
	}
	
	return ans;
}

double SINh(double ang){
	double P1=(exp(ang)-exp(-ang))/2.0;
	return P1;
}

double COSh(double ang){
	double P1=(exp(ang)+exp(-ang))/2.0;
	return P1;
}

double Asin(double P){
	if(P>1.0){
		P=1.0;
	}else if(P<-1.0){
		P=-1.0;
	}
	double ang=asin(P);
	return ang;
}

double Acos(double P){
	if(P>1.0){
		P=1.0;
	}else if(P<-1.0){
		P=-1.0;
	}
	double ang=acos(P);
	return ang;
}

//************************* 終端開放関数 *************************//
double end_free_func(double x0, double y0, double x1, double y1, double dy_dx, double x){
	double a_bra, b_bra, c_bra, d_bra;
	double y_bra;
	
	a_bra=dy_dx/pow(x1-x0, 2.0)-2.0*(y1-y0)/pow(x1-x0, 3.0);
	b_bra=-3.0/2.0*a_bra*(x1+x0)+dy_dx/(2.0*(x1-x0));
	c_bra=-3.0*a_bra*pow(x0, 2.0)-2.0*b_bra*x0;
	d_bra=y0-a_bra*pow(x0, 3.0)-b_bra*pow(x0, 2.0)-c_bra*x0;
	
	if(x<x0){
		y_bra=y0;
	}else if(x>x1){
		y_bra=y1;
	}else{
		y_bra=a_bra*pow(x, 3.0)+b_bra*pow(x, 2.0)+c_bra*x+d_bra;
	}
	
	return y_bra;
}

/*===========================================================
	cubic polynomial	(b1,b2は(x1,y1),(x2,y2)における傾き）
===========================================================*/
double cubic_polynomial_a0(double x1,double y1,double b1,double x2,double y2,double b2)
{
	double a0;
	a0=(x1*x1*((b2-b1)*x2*x2-3*x2*y2)+x1*x1*x1*(y2-b2*x2)+(3*x1*x2*x2-x2*x2*x2)*y1+b1*x1*x2*x2*x2)/(x1-x2)/(x1-x2)/(x1-x2);
	return a0;
}

double cubic_polynomial_a1(double x1,double y1,double b1,double x2,double y2,double b2)
{
	double a1;
	a1=-(x1*((2*b2+b1)*x2*x2-6*x2*y2)+6*x1*x2*y1+b1*x2*x2*x2+(-b2-2*b1)*x1*x1*x2-b2*x1*x1*x1)/(x1-x2)/(x1-x2)/(x1-x2);
	return a1;
}

double cubic_polynomial_a2(double x1,double y1,double b1,double x2,double y2,double b2)
{
	double a2;
	a2=(-3*x2*y2+x1*((b2-b1)*x2-3*y2)+(3*x2+3*x1)*y1+(b2+2*b1)*x2*x2+(-2*b2-b1)*x1*x1)/(x1-x2)/(x1-x2)/(x1-x2);
	return a2;
}

double cubic_polynomial_a3(double x1,double y1,double b1,double x2,double y2,double b2)
{
	double a3;
	a3=-(-2*y2+2*y1+(b2+b1)*x2+(-b2-b1)*x1)/(x1-x2)/(x1-x2)/(x1-x2);
	return a3;
}
