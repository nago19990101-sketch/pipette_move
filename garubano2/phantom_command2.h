#pragma once

#include "windows.h"
#include "string"
#include <tchar.h>

#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>

void PhantomOpen();
void PhantomClose();
HDCallbackCode HDCALLBACK GetDevicePosition(void* pUserData);
HDCallbackCode HDCALLBACK SetDeviceForce(void* pUserData);
HDCallbackCode HDCALLBACK GetButtonState(void* pUserData);