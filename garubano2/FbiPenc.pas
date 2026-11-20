unit FbiPenc;

interface

uses
  Windows;

{$IFDEF CONDITIONALEXPRESSIONS}
{$IF RTLVersion >= 15.00}
{$WARN UNSAFE_TYPE OFF}
{$WARN UNSAFE_CODE OFF}
{$WARN UNSAFE_CAST OFF}
{$IFEND}
{$ENDIF}

///////////////////////////////////////////////////////////////////////////////
//
//  Windows XP/2000/NT/Me/98/95
//  FbiPenc.pas   Include file for Interface encoder pulse counter board driver
//  Function Definition and Constant Definition  Copyright 2002,2003 Interface Corporation. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////

const


// -----------------------------------------------------------------------
//	Open flag identifier
// -----------------------------------------------------------------------
  PENC_FLAG_NORMAL = $0000;
  PENC_FLAG_SHARE  = $0002;

// -----------------------------------------------------------------------
//	Set/Get Filter
// -----------------------------------------------------------------------
  PENC_IN1_8       = $0100;
  PENC_IN9_16      = $0200;

// -----------------------------------------------------------------------
//	Set/Get Event
// -----------------------------------------------------------------------
  PENC_EVENT_BOARD = $0100;
  PENC_EVENT_DIO   = $0200;

// -----------------------------------------------------------------------
//	Return value
// -----------------------------------------------------------------------
  PENC_ERROR_SUCCESS		 =	$0;		// The operation is successfully completed.
  PENC_ERROR_NOT_DEVICE		 =	$C0000001;	// The specified device is not found.
  PENC_ERROR_NOT_OPEN		 =	$C0000002;	// The system cannot open the device.
  PENC_ERROR_INVALID_HANDLE	 =	$C0000003;	// The device handle is invalid.
  PENC_ERROR_ALREADY_OPEN	 =	$C0000004;      // The device has already been opened.
  PENC_ERROR_HANDLE_EOF		 =	$C0000005;	// EOF reached.
  PENC_ERROR_MORE_DATA		 =	$C0000006;	// More data is available.
  PENC_ERROR_INSUFFICIENT_BUFFER =      $C0000007;	// The data area passed to system call is too small.
  PENC_ERROR_IO_PENDING		 =	$C0000008;	// Overlapped I/O operations are in progress.
  PENC_ERROR_NOT_SUPPORTED	 =	$C0000009;	// The function is not supported.
  PENC_ERROR_INITIALIZE_IRQ	 =	$C0001000;	// Failed to initialize the interrupt function.
  PENC_ERROR_INVALID_CHANNEL	 =	$C0001001;	// The specified channel number is invalid.
  PENC_ERROR_INVALID_MODE	 =	$C0001002;	// The specified operation mode is invalid.
  PENC_ERROR_INVALID_DIRECT	 =	$C0001003;	// The specified count direction is invalid.
  PENC_ERROR_INVALID_EQUALS	 =	$C0001004;	// The specified setting value of comparator output is invalid.
  PENC_ERROR_INVALID_LATCH	 =	$C0001005;	// The specified latch setting value is invalid.
  PENC_ERROR_INVALID_COUNTER	 =	$C0001006;	// The specified counter value is invalid
  PENC_ERROR_INVALID_COMPARATOR  =	$C0001007;	// The specified comparator reference value is invalid
  PENC_ERROR_INVALID_ZMODE	 =	$C0001008;	// The specified polarity of phase Z, counter clear condition, and counter latch condition are invalid.
  PENC_ERROR_INVALID_MASK	 =	$C0001009;	// The specified event mask is invalid.
  PENC_ERROR_INVALID_ITIMER	 =	$C000100A;	// The specified interval timer count is invalid
  PENC_ERROR_ALREADY_REGISTRATION =	$C000100B;	// The event has already been registered
  PENC_ERROR_ALREADY_DELETE	 =	$C000100C;	// The event has already been deleted.
  PENC_ERROR_MEMORY_NOTALLOCATED =      $C000100D;	// Not enough memory
  PENC_ERROR_MEMORY_FREE	 =	$C000100E;	// Failed to release memory.
  PENC_ERROR_TIMER		 =	$C000100F;	// Failed to retrieve timer resources.
  PENC_ERROR_DRVCAL		 =	$C0001010;	// Failed to call the driver.
  PENC_ERROR_NULL_POINTER	 =	$C0001011;	// A NULL pointer is passed between the driver and DLL.
  PENC_ERROR_PARAMETER		 =	$C0001012;	// The parameter is invalid.

  PENC_ERROR_INVALID_FILTER		=	$C0001013;	// The specified digital filter is invalid.
  PENC_ERROR_INVALID_DO			=	$C0001014;	// The specified universal output data is invalid.
  PENC_ERROR_INVALID_RSTINMASK	=	$C0001015;	// The specified ResetIn mask is invalid.



  function PencOpen( lpszName: Pointer;   fdwFlags: DWORD ): THandle; stdcall; external 'FbiPenc.dll';
  function PencClose( hDeviceHandle: THandle ): Integer; stdcall; external 'FbiPenc.dll';
  function PencSetMode( hDeviceHandle: THandle;  nChannel: Integer;  nMode: Integer;  nDirection: Integer;  nEqual: Integer; nLatch: Integer ): Integer; stdcall; external 'FbiPenc.dll';
  function PencGetMode( hDeviceHandle: THandle;  nChannel: Integer;  var pnMode: Integer;  var pnDirection: Integer;  var pnEqual: Integer; var nLatch: Integer ): Integer; stdcall; external 'FbiPenc.dll';
  function PencSetCounter( hDeviceHandle: THandle;  nChannel: Integer;  dwCounter: DWORD ): Integer; stdcall; external 'FbiPenc.dll';
  function PencGetCounter( hDeviceHandle: THandle;  nChannel: Integer;  var pdwCounter: DWORD ): Integer; stdcall; external 'FbiPenc.dll';
  function PencSetComparator( hDeviceHandle: THandle;  nChannel: Integer;  dwComparator: DWORD ): Integer; stdcall; external 'FbiPenc.dll';
  function PencGetComparator( hDeviceHandle: THandle;  nChannel: Integer;  var pdwComparator: DWORD ): Integer; stdcall; external 'FbiPenc.dll';
  function PencSetZMode( hDeviceHandle: THandle;  nChannel: Integer;  nZMode: Integer ): Integer; stdcall; external 'FbiPenc.dll';
  function PencGetZMode( hDeviceHandle: THandle;  nChannel: Integer;  var pnZMode: Integer ): Integer; stdcall; external 'FbiPenc.dll';
  function PencSetEventMask( hDeviceHandle: THandle; nChannel:Integer; nEventMask: Integer; nTimerMask:Integer ): Integer; stdcall; external 'FbiPenc.dll';
  function PencGetEventMask( hDeviceHandle: THandle; nChannel:Integer; var nEventMask: Integer; var nTimerMask:Integer ): Integer; stdcall; external 'FbiPenc.dll';
  function PencSetEvent( hDeviceHandle: THandle;  lpEventProc: FARPROC;   dwUser: DWORD ): Integer; stdcall; external 'FbiPenc.dll';
  function PencKillEvent( hDeviceHandle: THandle ): Integer; stdcall; external 'FbiPenc.dll';
  function PencSetTimerConfig( hDeviceHandle: THandle;  bTimerConfig: BYTE ): Integer; stdcall; external 'FbiPenc.dll';
  function PencGetTimerConfig( hDeviceHandle: THandle;  var pbTimerConfig: BYTE ): Integer; stdcall; external 'FbiPenc.dll';
  function PencGetTimerCount( hDeviceHandle: THandle;  var pbTimerCount: BYTE ): Integer; stdcall; external 'FbiPenc.dll';
  function PencGetStatus( hDeviceHandle: THandle;  nChannel: Integer;  var pnStatus: Integer ): Integer; stdcall; external 'FbiPenc.dll';
  function PencReset( hDeviceHandle: THandle;  nChannel: Integer ): Integer; stdcall; external 'FbiPenc.dll';
  function PencSetCounterEx( hDeviceHandle: THandle;  dwChSel: Dword;  var pdwCounter: DWORD ): Integer; stdcall; external 'FbiPenc.dll';
  function PencGetCounterEx( hDeviceHandle: THandle;  dwChSel: Dword;  var pdwCounter: DWORD ): Integer; stdcall; external 'FbiPenc.dll';
  function PencEnableCount( hDeviceHandle: THandle; dwChSel: Dword; nEnable: Integer): Integer; stdcall; external 'FbiPenc.DLL';
  function PencSetEventEx( hDeviceHandle: THandle;  lpEventProcEx: FARPROC;   dwUser: DWORD ): Integer; stdcall; external 'FbiPenc.dll';
  function PencGetStatusEx( hDeviceHandle: THandle;  dwChSel: Dword;  var pdwCounter: Dword; var pdwStatus: Dword ): Integer; stdcall; external 'FbiPenc.dll';
  function PencEventRequestPending( hDeviceHandle: THandle; nChannel: Integer; dwEventMask: DWORD; var pdwEventBuf: DWORD; var pOverLapped: TOverlapped) : Integer; stdcall; external 'FbiPenc.DLL';

  function PencSetFilter( hDeviceHandle: THandle; nChannel: Integer; dwFilter: DWORD ) : Integer; stdcall; external 'FbiPenc.DLL';
  function PencGetFilter( hDeviceHandle: THandle; nChannel: Integer; var pdwFilter: DWORD ) : Integer; stdcall; external 'FbiPenc.DLL';
  function PencInputDI( hDeviceHandle: THandle; var pdwValue: DWORD ) : Integer; stdcall; external 'FbiPenc.DLL';
  function PencOutputDO( hDeviceHandle: THandle; dwValue: DWORD ) : Integer; stdcall; external 'FbiPenc.DLL';
  function PencSetResetInMask( hDeviceHandle: THandle; bResetInMask: BYTE ) : Integer; stdcall; external 'FbiPenc.DLL';
  function PencGetResetInMask( hDeviceHandle: THandle; pbResetInMask: BYTE ) : Integer; stdcall; external 'FbiPenc.DLL';
  function PencGetResetInStatus( hDeviceHandle: THandle; var pbResetInStatus: BYTE ) : Integer; stdcall; external 'FbiPenc.DLL';

implementation

end.
