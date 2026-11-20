// -----------------------------------------------------------------------
//
//
//   Windows XP/2000/NT/Me/98/95    FbiPenc.h   Include file for Interface encoder pulse counter board driver
//   Function Definition and Constant Definition   Copyright 1999, 2003 Interface Corporation. All rights reserved.
// -----------------------------------------------------------------------
#include	<Winioctl.h>

#if !defined( _FBIPENC_H_ )
#define _FBIPENC_H_

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------
//	Callback routine
//
//
// -----------------------------------------------------------------------
typedef void (CALLBACK PENCCALLBACK)(DWORD dwPencEvent, DWORD dwUser);
typedef PENCCALLBACK *LPPENCCALLBACK;

typedef void (CALLBACK PENCCALLBACKEX)(INT nChannel,DWORD dwPencEvent,DWORD dwUser);
typedef PENCCALLBACKEX *LPPENCCALLBACKEX;

// -----------------------------------------------------------------------
//	Identifier
// -----------------------------------------------------------------------
//	Open flag
#define	PENC_FLAG_NORMAL		0x0000
#define	PENC_FLAG_SHARE			0x0002

//	Set/Get Filter
#define	PENC_IN1_8				0x0100
#define	PENC_IN9_16				0x0200

//	Set/Get Event
#define	PENC_EVENT_BOARD		0x0100
#define	PENC_EVENT_DIO			0x0200
#define	PENC_EVENT_IRIN			0x0400

// -----------------------------------------------------------------------
//	Return value

// -----------------------------------------------------------------------
#define	PENC_ERROR_SUCCESS				0			// The operation is successfully completed.
#define PENC_ERROR_NOT_DEVICE			0xC0000001	// The specified device is not found.
#define	PENC_ERROR_NOT_OPEN				0xC0000002	// The system cannot open the device.
#define PENC_ERROR_INVALID_HANDLE		0xC0000003	// The device handle is invalid.
#define PENC_ERROR_ALREADY_OPEN			0xC0000004  // The device has already been opened.
#define PENC_ERROR_HANDLE_EOF			0xC0000005	// EOF reached.
#define PENC_ERROR_MORE_DATA			0xC0000006	// More data is available.
#define PENC_ERROR_INSUFFICIENT_BUFFER	0xC0000007	// The data area passed to system call is too small.
#define PENC_ERROR_IO_PENDING			0xC0000008	// Overlapped I/O operations are in progress.
#define PENC_ERROR_NOT_SUPPORTED		0xC0000009	// The function is not supported.
#define PENC_ERROR_INITIALIZE_IRQ		0xC0001000	// Failed to initialize the interrupt function.
#define	PENC_ERROR_INVALID_CHANNEL		0xC0001001	// The specified channel number is invalid.
#define	PENC_ERROR_INVALID_MODE			0xC0001002	// The specified operation mode is invalid.
#define	PENC_ERROR_INVALID_DIRECT		0xC0001003	// The specified count direction is invalid
#define	PENC_ERROR_INVALID_EQUALS		0xC0001004	// The specified setting value of comparator output is invalid
#define	PENC_ERROR_INVALID_LATCH		0xC0001005	// The specified latch setting value is invalid.
#define	PENC_ERROR_INVALID_COUNTER		0xC0001006	// The specified counter value is invalid
#define	PENC_ERROR_INVALID_COMPARATOR	0xC0001007	// The specified comparator reference value is invalid.
#define	PENC_ERROR_INVALID_ZMODE		0xC0001008	// The specified polarity of phase Z, counter clear condition, and counter latch condition are invalid
#define	PENC_ERROR_INVALID_MASK			0xC0001009	// The specified event mask is invalid.
#define	PENC_ERROR_INVALID_ITIMER		0xC000100A	// The specified interval timer count is invalid.
#define PENC_ERROR_ALREADY_REGISTRATION	0xC000100B	// The event has already been registered.
#define PENC_ERROR_ALREADY_DELETE		0xC000100C	// The event has already been deleted.
#define	PENC_ERROR_MEMORY_NOTALLOCATED	0xC000100D	// Not enough memory
#define	PENC_ERROR_MEMORY_FREE			0xC000100E	// Failed to release memory.
#define	PENC_ERROR_TIMER				0xC000100F	// Failed to retrieve timer resources.
#define	PENC_ERROR_DRVCAL				0xC0001010	// Failed to call the driver.
#define	PENC_ERROR_NULL_POINTER			0xC0001011	// A NULL pointer is passed between the driver and DLL.
#define PENC_ERROR_PARAMETER			0xC0001012	// The parameter is invalid.

#define	PENC_ERROR_INVALID_FILTER		0xC0001013	// The specified digital filter is invalid.
#define	PENC_ERROR_INVALID_DO			0xC0001014	// The specified universal output data is invalid.
#define	PENC_ERROR_INVALID_RSTINMASK	0xC0001015	// The specified ResetIn mask is invalid.

#if !defined(_FBIPENCLIB_)
#define FBIPENCAPI
#else
#define FBIPENCAPI __declspec(dllexport)
#endif

FBIPENCAPI HANDLE WINAPI PencOpen(LPCTSTR lpszName,DWORD fdwFlags);
FBIPENCAPI INT WINAPI PencClose(HANDLE hDevice);
FBIPENCAPI INT WINAPI PencSetMode(HANDLE hDevice,INT nChannel,INT nMode,INT nDirection,INT nEqual,INT nLatch);
FBIPENCAPI INT WINAPI PencGetMode(HANDLE hDevice,INT nChannel,PINT pnMode,PINT pnDirection,PINT nEqual,PINT nLatch);
FBIPENCAPI INT WINAPI PencSetCounter(HANDLE hDevice,INT nChannel,DWORD dwCounter);
FBIPENCAPI INT WINAPI PencGetCounter(HANDLE hDevice,INT nChannel,PDWORD pdwCounter);
FBIPENCAPI INT WINAPI PencSetCounterEx(HANDLE hDevice,DWORD dwChSel,PDWORD pdwCounter);
FBIPENCAPI INT WINAPI PencGetCounterEx(HANDLE hDevice,DWORD dwChSel,PDWORD pdwCounter);
FBIPENCAPI INT WINAPI PencSetComparator(HANDLE hDevice,INT nChannel,DWORD dwCounter);
FBIPENCAPI INT WINAPI PencGetComparator(HANDLE hDevice,INT nChannel,PDWORD pdwCounter);
FBIPENCAPI INT WINAPI PencSetZMode(HANDLE hDevice,INT nChannel,INT nZMode);
FBIPENCAPI INT WINAPI PencGetZMode(HANDLE hDevice,INT nChannel,PINT pnZMode);
FBIPENCAPI INT WINAPI PencGetStatus(HANDLE hDevice,INT nChannel,PINT pnStatus);
FBIPENCAPI INT WINAPI PencGetStatusEx(HANDLE hDevice,DWORD dwChSel,PDWORD pdwCounter,PDWORD pdwStatus);
FBIPENCAPI INT WINAPI PencEnableCount(HANDLE hDevice,DWORD dwChSel,INT nEnable);
FBIPENCAPI INT WINAPI PencReset(HANDLE hDevice,INT nChannel);
FBIPENCAPI INT WINAPI PencSetEventMask(HANDLE hDevice,INT nChannel,INT nEventMask,INT nTimerMask);
FBIPENCAPI INT WINAPI PencGetEventMask(HANDLE hDevice,INT nChannel,PINT pnEventMask,PINT pnTimerMask);
FBIPENCAPI INT WINAPI PencSetEvent(HANDLE hDevice,LPPENCCALLBACK lpEventProc,DWORD dwUser);
FBIPENCAPI INT WINAPI PencSetEventEx(HANDLE hDevice,LPPENCCALLBACKEX lpEventProcEx,DWORD dwUser);
FBIPENCAPI INT WINAPI PencKillEvent(HANDLE hDevice);
FBIPENCAPI INT WINAPI PencEventRequestPending(HANDLE hDevice,INT nChannel,DWORD dwEventMask,PDWORD pdwEventBuf,LPOVERLAPPED lpOverlapped);
FBIPENCAPI INT WINAPI PencSetTimerConfig(HANDLE hDevice,BYTE bTimerConfig);
FBIPENCAPI INT WINAPI PencGetTimerConfig(HANDLE hDevice,PBYTE pbTimerConfig);
FBIPENCAPI INT WINAPI PencGetTimerCount(HANDLE hDevice,PBYTE pbTimerCount);

FBIPENCAPI INT WINAPI PencSetFilter(HANDLE hDeviceHandle,INT nChannel,DWORD dwFilter);
FBIPENCAPI INT WINAPI PencGetFilter(HANDLE hDeviceHandle,INT nChannel,PDWORD pdwFilter);
FBIPENCAPI INT WINAPI PencInputDI(HANDLE hDeviceHandle,PDWORD pdwValue);
FBIPENCAPI INT WINAPI PencOutputDO(HANDLE hDeviceHandle,DWORD dwValue);
FBIPENCAPI INT WINAPI PencSetResetInMask(HANDLE hDeviceHandle,BYTE bResetInMask);
FBIPENCAPI INT WINAPI PencGetResetInMask(HANDLE hDeviceHandle,PBYTE pbResetInMask);
FBIPENCAPI INT WINAPI PencGetResetInStatus(HANDLE hDeviceHandle,PBYTE pbResetInStatus);
#ifdef __cplusplus
}
#endif

#endif

//.FBIPENC.H
