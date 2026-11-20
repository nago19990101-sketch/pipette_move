#include "phantom_command2.h"

#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#include <cstdio>
#include <cassert>

#if defined(WIN32)
# include <conio.h>
#else
# include "conio.h"
#endif

#include <hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>

HHD hHD;

void PhantomOpen() {

	HDErrorInfo error;

	// Initialize the default haptic device.
	hHD = hdInitDevice(HD_DEFAULT_DEVICE);
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to initialize haptic device");
		fprintf(stderr, "\nPress any key to quit.\n");
		getch();
		return;
	}

	// Start the servo scheduler and enable forces.
	hdEnable(HD_FORCE_OUTPUT);
	hdStartScheduler();
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to start the scheduler");
		fprintf(stderr, "\nPress any key to quit.\n");
		getch();
		return;
	}
}

void PhantomClose() {
	hdStopScheduler();
	hdDisableDevice(hHD);
}

HDCallbackCode HDCALLBACK GetDevicePosition(void* pUserData)
{
	HDdouble* pPosition = (HDdouble*)pUserData;

	hdBeginFrame(hdGetCurrentDevice());
	hdGetDoublev(HD_CURRENT_POSITION, pPosition);
	hdEndFrame(hdGetCurrentDevice());

	HDErrorInfo error;
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Error during phantom position callback\n");

		if (hduIsSchedulerError(&error))
		{
			return HD_CALLBACK_DONE;
		}
	}

	return HD_CALLBACK_DONE;
}

HDCallbackCode HDCALLBACK SetDeviceForce(void* pUserData) {

	HDdouble* f = (HDdouble*)pUserData;

	hdBeginFrame(hdGetCurrentDevice());
	hdSetDoublev(HD_CURRENT_FORCE, f);
	hdEndFrame(hdGetCurrentDevice());

	HDErrorInfo error;
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Error during phantom force callback\n");

		if (hduIsSchedulerError(&error))
		{
			return HD_CALLBACK_DONE;
		}
	}

	return HD_CALLBACK_DONE;
}

HDCallbackCode HDCALLBACK GetButtonState(void* pUserData) {

	HDint* buttonState = (HDint*)pUserData;

	hdBeginFrame(hdGetCurrentDevice());
	hdGetIntegerv(HD_CURRENT_BUTTONS, buttonState);
	hdEndFrame(hdGetCurrentDevice());

	HDErrorInfo error;
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Error during phantom button callback\n");

		if (hduIsSchedulerError(&error))
		{
			return HD_CALLBACK_DONE;
		}
	}

	return HD_CALLBACK_DONE;
}