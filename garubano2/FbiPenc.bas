Attribute VB_Name = "FbiPenc"
'///////////////////////////////////////////////////////////////////////////////
'//
'//  Windows XP/2000/NT/Me/98/95     FbiPenc.bas     Include file for Interface encoder pulse counter board driver
'//
'//          Function Definition and Constant Definition    Copyright 2002 Interface Corporation. All rights reserved.
'//
'///////////////////////////////////////////////////////////////////////////////


' -----------------------------------------------------------------------
'  Open flag identifier
' -----------------------------------------------------------------------
Public Const PENC_FLAG_NORMAL = &H0
Public Const PENC_FLAG_SHARE = &H2

' -----------------------------------------------------------------------
'  Set/Get Filter
' -----------------------------------------------------------------------
Public Const PENC_IN1_8 = &H0100
Public Const PENC_IN9_16 = &H0200

' -----------------------------------------------------------------------
'  Set/Get Event
' -----------------------------------------------------------------------
Public Const PENC_EVENT_BOARD = &H0100
Public Const PENC_EVENT_DIO = &H0200

' -----------------------------------------------------------------------
'  Return value
' -----------------------------------------------------------------------
Public Const PENC_ERROR_SUCCESS = &H0                        ' The operation is successfully completed.
Public Const PENC_ERROR_NOT_DEVICE = &HC0000001              ' The specified device is not found.
Public Const PENC_ERROR_NOT_OPEN = &HC0000002                ' The system cannot open the device.
Public Const PENC_ERROR_INVALID_HANDLE = &HC0000003          ' The device handle is invalid.
Public Const PENC_ERROR_ALREADY_OPEN = &HC0000004            ' The device has already been opened.
Public Const PENC_ERROR_HANDLE_EOF = &HC0000005              ' EOF reached.
Public Const PENC_ERROR_MORE_DATA = &HC0000006               ' More data is available.
Public Const PENC_ERROR_INSUFFICIENT_BUFFER = &HC0000007     ' The data area passed to system call is too small.
Public Const PENC_ERROR_IO_PENDING = &HC0000008              ' Overlapped I/O operations are in progress.
Public Const PENC_ERROR_NOT_SUPPORTED = &HC0000009           ' The function is not supported.
Public Const PENC_ERROR_INITIALIZE_IRQ = &HC0001000          ' Failed to initialize the interrupt function.
Public Const PENC_ERROR_INVALID_CHANNEL = &HC0001001         ' The specified channel number is invalid.
Public Const PENC_ERROR_INVALID_MODE = &HC0001002            ' The specified operation mode is invalid.
Public Const PENC_ERROR_INVALID_DIRECT = &HC0001003          ' The specified count direction is invalid.
Public Const PENC_ERROR_INVALID_EQUALS = &HC0001004          ' The specified setting value of comparator output is invalid.
Public Const PENC_ERROR_INVALID_LATCH = &HC0001005           ' The specified latch setting value is invalid.
Public Const PENC_ERROR_INVALID_COUNTER = &HC0001006         ' The specified counter value is invalid.
Public Const PENC_ERROR_INVALID_COMPARATOR = &HC0001007      ' The specified comparator reference value is invalid.
Public Const PENC_ERROR_INVALID_ZMODE = &HC0001008           ' The specified polarity of phase Z, counter clear condition, and counter latch condition are invalid.
Public Const PENC_ERROR_INVALID_MASK = &HC0001009            ' The specified event mask is invalid.
Public Const PENC_ERROR_INVALID_ITIMER = &HC000100A          ' The specified interval timer count is invalid.
Public Const PENC_ERROR_ALREADY_REGISTRATION = &HC000100B    ' The event has already been registered.
Public Const PENC_ERROR_ALREADY_DELETE = &HC000100C          ' The event has already been deleted.
Public Const PENC_ERROR_MEMORY_NOTALLOCATED = &HC000100D     ' Not enough memory
Public Const PENC_ERROR_MEMORY_FREE = &HC000100E             ' Failed to release memory.
Public Const PENC_ERROR_TIMER = &HC000100F                   ' Failed to retrieve timer resources.
Public Const PENC_ERROR_DRVCAL = &HC0001010                  ' Failed to call the driver.
Public Const PENC_ERROR_NULL_POINTER = &HC0001011            ' A NULL pointer is passed between the driver and DLL.
Public Const PENC_ERROR_PARAMETER = &HC0001012               ' The parameter is invalid.

Public Const PENC_ERROR_INVALID_FILTER = &HC0001013          ' The specified digital filter is invalid.
Public Const PENC_ERROR_INVALID_DO = &HC0001014              ' The specified universal output data is invalid.
Public Const PENC_ERROR_INVALID_RSTINMASK = &HC0001015       ' The specified ResetIn mask is invalid.

' -----------------------------------------------------------------------
'  Function Declaration for Visual Basic
' -----------------------------------------------------------------------
Declare Function PencOpen Lib "FbiPenc.DLL" (ByVal lpszName As String, ByVal fdwFlags As Long) As Long
Declare Function PencClose Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long) As Long
Declare Function PencSetMode Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByVal nMode As Long, ByVal nDirection As Long, ByVal nEqual As Long, ByVal nLatch As Long) As Long
Declare Function PencGetMode Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByRef pnMode As Long, ByRef pnDirection As Long, ByRef pnEqual As Long, ByRef pnLatch As Long) As Long
Declare Function PencSetCounter Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByVal dwCounter As Long) As Long
Declare Function PencGetCounter Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByRef pdwCounter As Long) As Long
Declare Function PencSetComparator Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByVal dwComparator As Long) As Long
Declare Function PencGetComparator Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByRef pdwComparator As Long) As Long
Declare Function PencSetZMode Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByVal nZMode As Long) As Long
Declare Function PencGetZMode Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByRef pnZMode As Long) As Long
Declare Function PencSetEventMask Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByVal nEventMask As Long, ByVal nTimerMask As Long) As Long
Declare Function PencGetEventMask Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByRef pnEventMask As Long, ByRef pnTimerMask As Long) As Long
Declare Function PencSetEvent Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal lpEventProc As Long, ByVal dwUser As Long) As Long
Declare Function PencKillEvent Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long) As Long
Declare Function PencSetTimerConfig Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal bTimerConfig As Byte) As Long
Declare Function PencGetTimerConfig Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByRef pbTimerConfig As Byte) As Long
Declare Function PencGetTimerCount Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByRef pbTimerCount As Byte) As Long
Declare Function PencGetStatus Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByRef pnStatus As Long) As Long
Declare Function PencReset Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long) As Long

Declare Function PencSetCounterEx Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal dwChSel As Long, ByRef pdwCounter As Long) As Long
Declare Function PencGetCounterEx Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal dwChSel As Long, ByRef pdwCounter As Long) As Long
Declare Function PencEnableCount Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal dwChSel As Long, ByVal nEnable As Long) As Long
Declare Function PencSetEventEx Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal lpEventProcEx As Long, ByVal dwUser As Long) As Long
Declare Function PencEventRequestPending Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByVal dwEventMask As Long, ByRef pdwEventBuf As Long, ByRef pOverLapped As OVERLAPPED) As Long
Declare Function PencGetStatusEx Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal dwChSel As Long, ByRef pdwCounter As Long, ByRef pdwStatus As Long) As Long

Declare Function PencSetFilter Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByVal dwFilter As Long) As Long
Declare Function PencGetFilter Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal nChannel As Long, ByRef pdwFilter As Long) As Long
Declare Function PencInputDI Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByRef pdwValue As Long) As Long
Declare Function PencOutputDO Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal dwValue As Long) As Long
Declare Function PencSetResetInMask Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByVal bResetInMask As Byte) As Long
Declare Function PencGetResetInMask Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByRef pbResetInMask As Byte) As Long
Declare Function PencGetResetInStatus Lib "FbiPenc.DLL" (ByVal hDeviceHandle As Long, ByRef pbResetInStatus As Byte) As Long

