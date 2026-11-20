#include "microsupportcommand.hpp"

void FirstSet(HANDLE hdevice) {

	//hdevice = McsdOpen((LPCTSTR)szBoadName, 0);//ボードオープン
	MCSDSPDDATA pSpdData = { 0,RANGEDATA, 8191,2,8191 };//初期速度4000μｍ/ｓ,最低速度１μｍ/ｓ
	McsdSetSpeed(hdevice, 0, &pSpdData);
	McsdSetSpeed(hdevice, 1, &pSpdData);
	McsdSetSpeed(hdevice, 2, &pSpdData);

	//McsdSetCounter(hdevice, 0, 0,50000);
	//McsdSetCounter(hdevice, 1, 0,50000);
	//McsdSetCounter(hdevice, 2, 0,50000);

}

void Getposition(HANDLE hdevice, double &x, double &y, double &z) {
	DWORD x_pulse=0;
	DWORD y_pulse=0;
	DWORD z_pulse=0;
	int Int = 0;

	McsdGetCounter(hdevice,0,Int,&x_pulse);
	McsdGetCounter(hdevice,1,Int,&y_pulse);
	McsdGetCounter(hdevice,2,Int,&z_pulse);
	
	x = x_pulse * (0.05);
	y = y_pulse * (-0.05);
	z = z_pulse * (-0.05);
}

void ChangeSpeed(HANDLE hdevice, std::vector<double>Highspeed) {
	McsdHiSpeedOverride(hdevice, 0, 1, Highspeed[0] * 20);
	McsdHiSpeedOverride(hdevice, 1, 1, Highspeed[1] * 20);
	McsdHiSpeedOverride(hdevice, 2, 1, Highspeed[2] * 20);
}

void Moveto(HANDLE hdevice, std::vector<double> speed, std::vector<double> amount) {
	DWORD er;
	WORD direction[3] = {0,1,1};
	//WORD status;

	if (amount[0] < 0) {
		direction[0] = 1;
		amount[0] *= -1;
		//std::cout << direction[0] << std::endl;
	}
	//else {
	//	direction[0] = 0;
	//}
	if (amount[1] < 0) {
		direction[1] = 0;
		amount[1] *= -1;
	}
	//else {
	//	direction[1] = 1;
	//}
	if (amount[2] < 0) {
		direction[2] = 0;
		amount[2] *= -1;
	}
	//else {
	//	direction[2] = 1;
	//}

	//動いてたら止める
	//McsdGetAxisBusy(hdevice, &status);//状態取得
	//if(((status >> 0) & 1)==1)McsdDriveStop(hdevice, 0, 0);
	//if(((status >> 1) & 1)==1)McsdDriveStop(hdevice, 1, 0);
	//if(((status >> 2) & 1)==1)McsdDriveStop(hdevice, 2, 0);

	McsdSyncStart(hdevice, 1);//複数軸同期信号待機

	if (speed[0] == 0 || amount[0] == 0) {
		McsdDriveStop(hdevice, 0, 0);
	}
	else {
		McsdHiSpeedOverride(hdevice, 0, 1, speed[0] * 20);
		er=McsdDriveStart(hdevice, 0, direction[0], amount[0] * 20);
		//std::cout << er << std::endl;
	}
	if (speed[1] == 0 || amount[1] == 0) {
		McsdDriveStop(hdevice, 1, 0);
	}
	else {
		McsdHiSpeedOverride(hdevice, 1, 1, speed[1] * 20);
		McsdDriveStart(hdevice, 1, direction[1], amount[1] * 20);
	}
	if (speed[2] == 0 || amount[2] == 0) {
		McsdDriveStop(hdevice, 2, 0);
	}
	else {
		McsdHiSpeedOverride(hdevice, 2, 1, speed[2] * 20);
		McsdDriveStart(hdevice, 2, direction[2], amount[2] * 20);
	}

	McsdSyncStart(hdevice, 0);//複数軸同期スタート

}

void Moveto2(HANDLE hdevice, Vector3D speed, Vector3D amount) {
	DWORD er;
	WORD direction[3] = { 0,1,1 };
	//WORD status;

	if (amount.x < 0) {
		direction[0] = 1;
		amount.x *= -1;
		//std::cout << direction[0] << std::endl;
	}
	//else {
	//	direction[0] = 0;
	//}
	if (amount.y< 0) {
		direction[1] = 0;
		amount.y*= -1;
	}
	//else {
	//	direction[1] = 1;
	//}
	if (amount.z < 0) {
		direction[2] = 0;
		amount.z *= -1;
	}
	//else {
	//	direction[2] = 1;
	//}

	//動いてたら止める
	//McsdGetAxisBusy(hdevice, &status);//状態取得
	//if(((status >> 0) & 1)==1)McsdDriveStop(hdevice, 0, 0);
	//if(((status >> 1) & 1)==1)McsdDriveStop(hdevice, 1, 0);
	//if(((status >> 2) & 1)==1)McsdDriveStop(hdevice, 2, 0);

	McsdSyncStart(hdevice, 1);//複数軸同期信号待機

	if (speed.x == 0 || amount.x == 0) {
		McsdDriveStop(hdevice, 0, 0);
	}
	else {
		McsdHiSpeedOverride(hdevice, 0, 1, speed.x * 20);
		er = McsdDriveStart(hdevice, 0, direction[0], amount.x * 20);
		//std::cout << er << std::endl;
	}
	if (speed.y == 0 || amount.y == 0) {
		McsdDriveStop(hdevice, 1, 0);
	}
	else {
		McsdHiSpeedOverride(hdevice, 1, 1, static_cast<DWORD>(speed.y * 20));
		McsdDriveStart(hdevice, 1, direction[1], static_cast<DWORD>(amount.y * 20));
	}
	if (speed.z == 0 || amount.z == 0) {
		McsdDriveStop(hdevice, 2, 0);
	}
	else {
		McsdHiSpeedOverride(hdevice, 2, 1, static_cast<DWORD>(speed.z * 20));
		McsdDriveStart(hdevice, 2, direction[2], static_cast<DWORD>(amount.z * 20));
	}

	McsdSyncStart(hdevice, 0);//複数軸同期スタート

}

void Movepos(HANDLE hdevice, std::vector<double> speed, std::vector<double> position) {
	WORD status;
	std::vector<double> nowpos{0,0,0};
	std::vector<double> amounttomove{0,0,0};
	
	//動いてたら止める
	//McsdGetAxisBusy(hdevice, &status);//状態取得
	//if(((status >> 0) & 1)==1)McsdDriveStop(hdevice, 0, 0);
	//if(((status >> 1) & 1)==1)McsdDriveStop(hdevice, 1, 0);
	//if(((status >> 2) & 1)==1)McsdDriveStop(hdevice, 2, 0);

	Getposition(hdevice, nowpos[0], nowpos[1], nowpos[2]);
	//std::cout << nowpos[0] << "," << nowpos[1] << "," << nowpos[2] << std::endl;

	for (int i = 0; i <= 2; i++) {
		amounttomove[i] = position[i] - nowpos[i];
	}
	//std::cout << amounttomove[0] << "," << amounttomove[1] << "," << amounttomove[2] << std::endl;

	Moveto(hdevice, speed, amounttomove);

}

void Movepos2(HANDLE hdevice, Vector3D speed, Vector3D position) {
	WORD status;
	Vector3D nowpos{ 0,0,0 };
	Vector3D amounttomove{ 0,0,0 };

	//動いてたら止める
	//McsdGetAxisBusy(hdevice, &status);//状態取得
	//if(((status >> 0) & 1)==1)McsdDriveStop(hdevice, 0, 0);
	//if(((status >> 1) & 1)==1)McsdDriveStop(hdevice, 1, 0);
	//if(((status >> 2) & 1)==1)McsdDriveStop(hdevice, 2, 0);

	Getposition(hdevice, nowpos.x, nowpos.y, nowpos.z);
	//std::cout << nowpos[0] << "," << nowpos[1] << "," << nowpos[2] << std::endl;

	amounttomove = position - nowpos;

	//for (int i = 0; i <= 2; i++) {
	//	amounttomove[i] = position[i] - nowpos[i];
	//}
	//std::cout << amounttomove[0] << "," << amounttomove[1] << "," << amounttomove[2] << std::endl;

	Moveto2(hdevice, speed, amounttomove);

}

void Go(HANDLE hdevice, std::vector<double> speed){
	//DWORD er;
	WORD direction[3];

	if (speed[0] < 0) {
		direction[0] = 1;
		speed[0] *= -1;
	}
	else {
		direction[0] = 0;
	}
	if (speed[1] < 0) {
		direction[1] = 0;
		speed[1] *= -1;
	}
	else {
		direction[1] = 1;
	}
	if (speed[2] < 0) {
		direction[2] = 0;
		speed[2] *= -1;
	}
	else {
		direction[2] = 1;
	}
	
	McsdSyncStart(hdevice, 1);//複数軸同期信号待機
	if (speed[0] == 0) {
		McsdDriveStop(hdevice, 0, 0);
	}
	else {
		McsdHiSpeedOverride(hdevice, 0, 1, speed[0] * 20);
		McsdDriveStart(hdevice, 0, direction[0] + 2, 0);
}
	if (speed[1] == 0) {
		McsdDriveStop(hdevice, 1, 0);
	}
	else {
		McsdHiSpeedOverride(hdevice, 1, 1, speed[1] * 20);
		McsdDriveStart(hdevice, 1, direction[1] + 2, 0);
	}	
	if (speed[2] == 0) {
		McsdDriveStop(hdevice, 2, 0);
	}
	else {
		McsdHiSpeedOverride(hdevice, 2, 1, speed[2] * 20);
		McsdDriveStart(hdevice, 2, direction[2] + 2, 0);
	}

	McsdSyncStart(hdevice, 0);//複数軸同期スタート
	

}

void DriveStop(HANDLE hdevice) {
	McsdDriveStop(hdevice, 0, 0);
	McsdDriveStop(hdevice, 1, 0);
	McsdDriveStop(hdevice, 2, 0);
}


