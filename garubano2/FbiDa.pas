unit FbiDa;

{$IFDEF CONDITIONALEXPRESSIONS}
{$IF RTLVersion >= 15.00}
{$WARN UNSAFE_TYPE OFF}
{$WARN UNSAFE_CODE OFF}
{$WARN UNSAFE_CAST OFF}
{$IFEND}
{$ENDIF}

interface


uses
  Windows, Dialogs, SysUtils,  Messages;

//-----------------------------------------------------------------------------------------------
//
//              Overlapped Process Identifier
//
//-----------------------------------------------------------------------------------------------
const
  FLAG_SYNC 	= 1;	// The analog output update is performed as an non-overlapped operation.
  FLAG_ASYNC	= 2;	// The analog output update is performed as an overlapped operation.


//-----------------------------------------------------------------------------------------------
//
//		File Format Identifier
//
//-----------------------------------------------------------------------------------------------
const
  FLAG_BIN	= 1;	// Binary format file
  FLAG_CSV	= 2;	// CSV format file


//-----------------------------------------------------------------------------------------------
//
//		Analog Output Status Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_STATUS_STOP_SAMPLING  = 1;	   // The analog output update is stopped.
  DA_STATUS_WAIT_TRIGGER   = 2;	   // The analog output update is waiting for a trigger.
  DA_STATUS_NOW_SAMPLING   = 3;	   // The analog output update is running.


//-----------------------------------------------------------------------------------------------
//
//		Event Factor Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_EVENT_STOP_TRIGGER    = 1;	   // The analog output has been stopped because a trigger is asserted.
  DA_EVENT_STOP_FUNCTION   = 2;	   // The analog output has been stopped by software.
  DA_EVENT_STOP_SAMPLING   = 3;	   // The analog output terminated.
  DA_EVENT_RESET_IN	       = 4;	   // The reset input signal is asserted.
  DA_EVENT_CURRENT_OFF     = 5;    // The current loop fault has been detected.
  DA_EVENT_COUNT           = 6;	   // The specified number of output has been acquired.
  DA_EVENT_FIFO_EMPTY      = 7;    // Fifo Empty
  DA_EVENT_EX_INT          = 8;    // EX INT
  DA_EVENT_EXOV_OFF        = 9;    // EX Over Voktage off
  DA_EVENT_OV_OFF          = 10;   // Over Voktage off


//-----------------------------------------------------------------------------------------------
//
//		Range Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_0_1V		   = $00000001;	// Voltage: unipolar 0 V to +1 V
  DA_0_2P5V		   = $00000002;	// Voltage: unipolar 0 V to +2.5 V
  DA_0_5V		   = $00000004;	// Voltage: unipolar 0 V to +5 V
  DA_0_10V		   = $00000008;	// Voltage: unipolar 0 V to +10 V
  DA_1_5V		   = $00000010;	// Voltage: unipolar +1 V to +5 V
  DA_0_20mA		   = $00001000;	// Current: unipolar 0 mA to +20 mA
  DA_4_20mA		   = $00002000;	// Current: unipolar +4 mA to +20 mA
  DA_0_1mA		   = $00004000;	// Current: unipolar 0 mA to +1 mA
  DA_0_100mA	   = $00008000;	// Current: unipolar 0 mA to +100 mA
  DA_1V			   = $00010000;	// Voltage: bipolar +/-1 V
  DA_2P5V		   = $00020000;	// Voltage: bipolar +/-2.5 V
  DA_5V			   = $00040000;	// Voltage: bipolar +/-5 V
  DA_10V		   = $00080000;	// Voltage: bipolar +/-10 V
  DA_20mA		   = $01000000;	// Current: bipolar +/-20 mA


//-----------------------------------------------------------------------------------------------
//
//		Data Transfer Architecture Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_IO_SAMPLING	   = 1;	   // Programmed I/O
  DA_FIFO_SAMPLING	   = 2;	   // FIFO
  DA_MEM_SAMPLING	   = 4;	   // Memory


//-----------------------------------------------------------------------------------------------
//
//		Trigger Point Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_TRIG_START		   = 1;	   // Start-trigger (default setting)
  DA_TRIG_STOP		   = 2;	   // top-trigger
  DA_TRIG_START_STOP	   = 3;	   // Start/stop-trigger


//-----------------------------------------------------------------------------------------------
//
//		Trigger Level Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_FREERUN		   = 1;	   // No trigger (default setting)
  DA_EXTTRG		   = 2;	   // External trigger
  DA_EXTTRG_DI		   = 3;	   // External trigger with DI masking


//-----------------------------------------------------------------------------------------------
//
//		Polarity Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_DOWN_EDGE		   = 1;	   // Falling edge (default setting)
  DA_UP_EDGE		   = 2;	   // Rising edge


//-----------------------------------------------------------------------------------------------
//
//		Isolation Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_ISOLATION		   = 1;	   // Photo-isolated board
  DA_NOT_ISOLATION	   = 2;	   // Not isolated board


//-----------------------------------------------------------------------------------------------
//
//		Range Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_RANGE_UNIPOLAR		= 1;	// Unipolar
  DA_RANGE_BIPOLAR		= 2;	// Bipolar

//-----------------------------------------------------------------------------------------------
//
//		Waveform Generation Mode Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_MODE_CUT			= 1;	// Time-based waveform generation
  DA_MODE_SYNTHE		= 2;	// Frequency-based waveform generation

//-----------------------------------------------------------------------------------------------
//
//		Repeat Mode Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_REPEAT_NONINTERVAL	= 1;	// Repeat without the wait state (default setting).
  DA_REPEAT_INTERVAL	= 2;	// Repeat with the wait state.

//-----------------------------------------------------------------------------------------------
//
//		Counter Clear Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_COUNTER_CLEAR	= 1;	// Cleared (default setting)
  DA_COUNTER_NONCLEAR	= 2;	// Not cleared

//-----------------------------------------------------------------------------------------------
//
//		DA Latch Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_LATCH_CLEAR		= 1;	// The voltage is set to the lowest voltage of the range.
  DA_LATCH_NONCLEAR		= 2;	// The voltage is held.

//-----------------------------------------------------------------------------------------------
//
//		Clock Source Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_CLOCK_TIMER		= 1;	// Internal programmable timer (8254 compatible)
  DA_CLOCK_FIXED		= 2;	// Fixed 5 MHz clock

//-----------------------------------------------------------------------------------------------
//
//		Configurations of the Connector CN3 Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_EXTRG_IN			= 1;	// External trigger input (default setting)
  DA_EXTRG_OUT			= 2;	// External trigger output

//-----------------------------------------------------------------------------------------------
//
//		Configurations of the Connector CN4 Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_EXCLK_IN			= 1;	// External clock input (default setting)
  DA_EXCLK_OUT			= 2;	// External clock output

//-----------------------------------------------------------------------------------------------
//
//		Reset Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_RESET_ON           = 1;	//Used
  DA_RESET_OFF          = 2;	// Not used (default setting)

//-----------------------------------------------------------------------------------------------
//
//		Filter Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_FILTER_OFF			= 1;	// Not used (default setting)
  DA_FILTER_ON			= 2;	// Used

//-----------------------------------------------------------------------------------------------
//
//		Synchronous Analog Output Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_MASTER_MODE		= 1;	// Master mode
  DA_SLAVE_MODE			= 2;	// Slave mode

//-----------------------------------------------------------------------------------------------
//
//     Synchronous Number Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_SYNC_NUM_1			= $0100;
  DA_SYNC_NUM_2			= $0200;
  DA_SYNC_NUM_3			= $0400;
  DA_SYNC_NUM_4			= $0800;
  DA_SYNC_NUM_5			= $1000;
  DA_SYNC_NUM_6			= $2000;
  DA_SYNC_NUM_7			= $4000;

//-----------------------------------------------------------------------------------------------
//
//		PCI-3525 External Trigger Polarity Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_START_DOWN_EDGE		= 1;	// Start external trigger falling edge
  DA_START_UP_EDGE		= 2;	// Start external trigger rising edge
  DA_STOP_DOWN_EDGE		= 4;	// Stop external trigger falling edge
  DA_STOP_UP_EDGE		= 8;	// Stop external trigger rising edge

//-----------------------------------------------------------------------------------------------
//
//		PCI-3525 Trigger Level Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_TRG_FREERUN		= 0;	// No trigger
  DA_TRG_EXTTRG			= 1;	// External trigger
  DA_TRG_ATRG			= 2;	// Analog trigger
  DA_TRG_SIGTIMER		= 3;	// Interval timer
  DA_TRG_CNT_EQ         = 4;    // Counter equal
  DA_TRG_Z_CLR          = 5;    // Z clear
  DA_TRG_AD_START		= 5;	// Analog input start
  DA_TRG_AD_STOP		= 6;	// Analog input stop
  DA_TRG_AD_PRETRG		= 7;	// Analog input pre-trigger
  DA_TRG_AD_POSTTRG		= 8;	// Analog input post-trigger
  DA_TRG_SMPLNUM		= 9;	// Analog output stop number
  DA_TRG_FIFO_EMPTY		= 10;	// FIFO empty
  DA_TRG_SYNC1			= 14;	// Internel sync1 trigger
  DA_TRG_SYNC2			= 15;	// Internel sync2 trigger
  DA_FIFORESET			= $0100;	// FIFO reset
  DA_RETRG				= $0200;	// Retrigger

//-----------------------------------------------------------------------------------------------
//
//		GPC/GPF-3100 CPZ-360810 DIN/DOUT Function Identifier
//
//-----------------------------------------------------------------------------------------------
const
   DA_EX_DIO1			= 1;	// DIN/DOUT1
   DA_EX_DIO2			= 2;	// DIN/DOUT2
   DA_EX_DIO3			= 3;	// DIN/DOUT3
   DA_EX_DIO4			= 4;	// DIN/DOUT4
   DA_EX_DIO5			= 5;	// DIN/DOUT5

//-----------------------------------------------------------------------------------------------
//
//		Simultaneous Output Set Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_NORMAL_OUTPUT		= 1;	// Not simultaneous output
  DA_SYNC_OUTPUT		= 2;	// Simultaneous output

//-----------------------------------------------------------------------------------------------
//
//		Volume Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_DAJUST_BIOFFSET	= 1;	// Bipolar offset calibration
  DA_DAJUST_UNIOFFSET	= 2;	// Unipolar offset calibration
  DA_DAJUST_BIGAIN		= 3;	// Bipolar gain calibration
  DA_DAJUST_UNIGAIN		= 4;	// Unipolar gain calibration


//-----------------------------------------------------------------------------------------------
//
//		Calibration Item Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_ADJUST_UP		   = 1;	   // Increase the volume.
  DA_ADJUST_DOWN	   = 2;	   // Decrease the volume.
  DA_ADJUST_STORE	   = 3;	   // Save the present value to the non-volatile memory.
  DA_ADJUST_STANDBY	   = 4;	   // Place the electronic volume device into the standby mode.
  DA_ADJUST_NOT_STORE	   = 5;    // Not save the value.


//-----------------------------------------------------------------------------------------------
//
//     Read Adjust Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_ADJUST_READ_FACTORY = 1;	// Factrory Setting
  DA_ADJUST_READ_USER = 2;		// User Setting


//-----------------------------------------------------------------------------------------------
//
//		Data Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_DATA_PHYSICAL	   = 1;	   // Physical value (voltage [V], current [mA])
  DA_DATA_BIN8		   = 2;	   // 8-bit binary
  DA_DATA_BIN12		   = 3;	   // 12-bit binary
  DA_DATA_BIN16		   = 4;	   // 16-bit binary
  DA_DATA_BIN24		   = 5;	   // 24-bit binary
  DA_DATA_BIN14		   = 6;	   // 14-bit binary

//-----------------------------------------------------------------------------------------------
//
//		Data Conversion Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_CONV_SMOOTH	   = 1;	   // Convert the data with interpolation.
  DA_CONV_AVERAGE1	   = $100; // Convert the data with the simple averaging.
  DA_CONV_AVERAGE2	   = $200; // Convert the data with the shifted averaging.


//-----------------------------------------------------------------------------------------------
//
//		Error Identifier
//
//-----------------------------------------------------------------------------------------------
const
  DA_ERROR_SUCCESS		        = $00000000;
  DA_ERROR_NOT_DEVICE		        = $C0000001;
  DA_ERROR_NOT_OPEN		        = $C0000002;
  DA_ERROR_INVALID_HANDLE	        = $C0000003;
  DA_ERROR_ALREDAY_OPEN		        = $C0000004;
  DA_ERROR_NOT_SUPPORTED	        = $C0000009;
  DA_ERROR_NOW_SAMPLING		        = $C0001001;
  DA_ERROR_STOP_SAMPLING	        = $C0001002;
  DA_ERROR_START_SAMPLING	        = $C0001003;
  DA_ERROR_SAMPLING_TIMEOUT	        = $C0001004;
  DA_ERROR_INVALID_PARAMETER	        = $C0001021;
  DA_ERROR_ILLEGAL_PARAMETER	        = $C0001022;
  DA_ERROR_NULL_POINTER		        = $C0001023;
  DA_ERROR_SET_DATA		        = $C0001024;
  DA_ERROR_USED_AD			= $C0001025;
  DA_ERROR_FILE_OPEN		        = $C0001041;
  DA_ERROR_FILE_CLOSE		        = $C0001042;
  DA_ERROR_FILE_REDA		        = $C0001043;
  DA_ERROR_FILE_WRITE		        = $C0001044;
  DA_ERROR_INVALID_DATA_FORMAT	        = $C0001061;
  DA_ERROR_INVALID_AVERAGE_OR_SMOOTHING	= $C0001062;
  DA_ERROR_INVALID_SOURCE_DATA	        = $C0001063;
  DA_ERROR_NOT_ALLOCATE_MEMORY	        = $C0001081;
  DA_ERROR_NOT_LODA_DLL		            = $C0001082;
  DA_ERROR_CALL_DLL                     = $C0001083;
  DA_ERROR_CALIBRATION                  = $C0001084;
  DA_ERROR_USBIO_FAILED                  = $C0001085;
  DA_ERROR_USBIO_TIMEOUT                  = $C0001086;

// -----------------------------------------------------------------------
//
//	Analog Output Request Condition Structure for Each Channel
//
// -----------------------------------------------------------------------
type
  DASMPLCHREQ = record
    ulChNo : DWORD;
    ulRange : DWORD;
  end;


// -----------------------------------------------------------------------
//
//	Analog Output Request Condition Structure
//
// -----------------------------------------------------------------------
type
  DASMPLREQ = record
    ulChCount : DWORD;
    SmplChReq : array[0..255] of DASMPLCHREQ;
    ulSamplingMode : DWORD;
    fSmplFreq : Single;
    ulSmplRepeat : DWORD;
    ulTrigMode : DWORD;
    ulTrigPoint : DWORD;
    ulTrigDelay : DWORD;
    ulEClkEdge : DWORD;
    ulTrigEdge : DWORD;
    ulTrigDI : DWORD;
  end;


// -----------------------------------------------------------------------
//
//	Board Specification Structure
//
// -----------------------------------------------------------------------
type
  DABOARDSPEC = record
    ulBoardType : DWORD;
    ulBoardID : DWORD;
    ulSamplingMode : DWORD;
    ulChCount : DWORD;
    ulResolution : DWORD;
    ulRange : DWORD;
    ulIsolation : DWORD;
    ulDi : DWORD;
    ulDo : DWORD;
  end;


// -----------------------------------------------------------------------
//	Output Range Configurations Structure for Each Channel (for the PCI/PAZ-3305)
// -----------------------------------------------------------------------
type
  DAMODECHREQ = record
    ulRange : DWORD;
    fVolt : Single;
    ulFilter : DWORD;
  end;

// -----------------------------------------------------------------------
//	Waveform Generation Mode Structure (for the PCI/PAZ-3305)
// -----------------------------------------------------------------------
type
  DAMODEREQ = record
	ModeChReq : array[0..1] of DAMODECHREQ;
	ulPulseMode : DWORD;
	ulSyntheOut : DWORD;
	ulInterval : DWORD;
	fIntervalCycle : Single;
	ulCounterClear : DWORD;
	ulDaLatch : DWORD;
	ulSamplingClock : DWORD;
	ulExControl : DWORD;
	ulExClock : DWORD;
  end;

// -----------------------------------------------------------------------
//	Fifo Analog Output Request Condition Structure (for the PCI-3525)
// -----------------------------------------------------------------------
type
   DAFIFOREQ = record
	ulChCount : DWORD;
	SmplChReq : array[0..255] of DASMPLCHREQ;
	fSmplFreq : Single;
	ulSmplRepeat : DWORD;
	ulSmplNum : DWORD;
	ulStartTrigCondition : DWORD;
	ulStopTrigCondition : DWORD;
	ulEClkEdge : DWORD;
	ulTrigEdge : DWORD;
  end;

function DaOpen(lpszName: Pointer): THandle; stdcall; external 'FbiDa.dll';
function DaClose(hDeviceHandle: THandle): Integer; stdcall; external 'FbiDa.dll';

function DaGetDeviceInfo(hDeviceHandle: THandle; var pDaBoardSpec: DABOARDSPEC): Integer; stdcall; external 'FbiDa.DLL';
function DaSetBoardConfig( hDeviceHandle: THandle; ulSmplBufferSize : Cardinal; hEvent: THandle; lpCallBackProc: FARPROC;   dwUser: DWORD ): Integer; stdcall; external 'FbiDa.dll';
function DaSetCountEvent( hDeviceHandle: THandle; ulEventNum : Cardinal; hEvent: THandle; lpCallBackProc: FARPROC;   dwUser: DWORD ): Integer; stdcall; external 'FbiDa.dll';
function DaGetBoardConfig( hDeviceHandle: THandle; var ulSmplBufferSize : Cardinal; var ulDaSmplEventFactor: Cardinal ): Integer; stdcall; external 'FbiDa.dll';

function DaSetSamplingConfig(hDeviceHandle: THandle; var pDaSmplConfig: DASMPLREQ): Integer; stdcall; external 'FbiDa.DLL';
function DaGetSamplingConfig(hDeviceHandle: THandle; var pDaSmplConfig: DASMPLREQ): Integer; stdcall; external 'FbiDa.DLL';

function DaSetMode(hDeviceHandle: THandle; var pDaMode: DAMODEREQ): Integer; stdcall; external 'FbiDa.DLL';
function DaGetMode(hDeviceHandle: THandle; var pDaMode: DAMODEREQ): Integer; stdcall; external 'FbiDa.DLL';

function DaSetSamplingData(hDeviceHandle: THandle; pSmplData: Pointer; ulSmplDataNum : Cardinal): Integer; stdcall; external 'FbiDa.DLL';
function DaClearSamplingData(hDeviceHandle: THandle): Integer; stdcall; external 'FbiDa.DLL';

function DaStartSampling(hDeviceHandle: THandle; ulSyncFlag : Cardinal ): Integer; stdcall; external 'FbiDa.DLL';
function DaStartFileSampling(hDeviceHandle: THandle; pszPathName: Pointer; ulFileFlag : Cardinal; ulSmplNum: Cardinal ): Integer; stdcall; external 'FbiDa.DLL';
function DaSyncSampling(hDeviceHandle: Integer; ulMode: Integer): DWORD; stdcall; external 'FbiDa.DLL';
function DaStopSampling(hDeviceHandle: THandle): Integer; stdcall; external 'FbiDa.DLL';

function DaGetStatus(hDeviceHandle: THandle; var ulDaSmplStatus : Cardinal; var ulDaSmplCount : Cardinal; var ulDaAvailCount : Cardinal; var ulDaAvailRepeat : Cardinal ): Integer; stdcall; external 'FbiDa.DLL';

function DaSetOutputMode(hDeviceHandle: THandle; ulMode : Integer ): Integer; stdcall; external 'FbiDa.DLL';
function DaGetOutputMode(hDeviceHandle: THandle; var ulMode : Integer ): Integer; stdcall; external 'FbiDa.DLL';

function DaOutputDA(hDeviceHandle: THandle; ulCh : Cardinal;  var pDaSmplChReq : DASMPLCHREQ;  pData: Pointer): Integer; stdcall; external 'FbiDa.DLL';

function DaInputDI(hDeviceHandle: THandle; var dwData : Integer ): Integer; stdcall; external 'FbiDa.DLL';
function DaOutputDO(hDeviceHandle: THandle; dwData : Integer ): Integer; stdcall; external 'FbiDa.DLL';

function DaAdjustVR(hDeviceHandle: THandle; ulAdjustCh: Cardinal; ulSelVolume: Cardinal; ulDirection: Cardinal; ulTap: Cardinal) : Integer; stdcall; external 'FbiDa.DLL';
function DaReadAdjustVR(hDeviceHandle: THandle; ulAdjustCh: Cardinal ) : Integer; stdcall; external 'FbiDa.DLL';
function DaReadAdjustVREx(hDeviceHandle: THandle; ulAdjustCh: Cardinal; ulControl: Cardinal ) : Integer; stdcall; external 'FbiDa.DLL';

function DaDataConv(uSrcFormCode: Cardinal; pSrcData: Pointer; uSrcSmplDataNum: Cardinal; var pSrcSmplReq: DASMPLREQ; uDestFormCode: Cardinal; pDestData: Pointer; var puDestSmplDataNum: Pointer; var pDestSmplReq: DASMPLREQ; uEffect: Cardinal; uCount: Cardinal; pfnConv : Pointer) : Integer; stdcall; external 'FbiDaDC.DLL';
function DaWriteFile(pszPathName: Pointer; pSmplData: Pointer; ulFormCode: Cardinal; ulSmplNum: Cardinal; ulChCount: Cardinal) : Integer; stdcall; external 'FbiDaDC.DLL';

function DaSetFifoConfig(hDeviceHandle : THandle; var pDaFifoConfig : DAFIFOREQ ) : Integer; stdcall; external 'FbiDa.DLL';
function DaGetFifoConfig(hDeviceHandle : THandle; var pDaFifoConfig : DAFIFOREQ ) : Integer; stdcall; external 'FbiDa.DLL';

function DaSetInterval(hDeviceHandle: THandle; ulInterval : Cardinal ) : DWORD; stdcall; external 'FbiDa.DLL';
function DaGetInterval(hDeviceHandle: THandle; var ulInterval : Cardinal ) : DWORD; stdcall; external 'FbiDA.DLL';

function DaSetFunction(hDeviceHandle: THandle; ulChNo : Cardinal; ulFunction : Cardinal) : DWORD; stdcall; external 'FbiDa.DLL';
function DaGetFunction(hDeviceHandle: THandle; ulChNo : Cardinal; var ulFunction : Cardinal) : DWORD; stdcall; external 'FbiDa.DLL';

function DaSetCurrentDir(hDeviceHandle: THandle; dwData : Integer ): Integer; stdcall; external 'FbiDa.DLL';
function DaGetCurrentDir(hDeviceHandle: THandle; var dwData : Integer ): Integer; stdcall; external 'FbiDa.DLL';
function DaSetPowerSupply(hDeviceHandle: THandle; ExOnOff : Integer ): Integer; stdcall; external 'FbiDa.DLL';
function DaGetPowerSupply(hDeviceHandle: THandle; var ExOnOff : Integer ): Integer; stdcall; external 'FbiDa.DLL';
function DaSetExcessVoltage(hDeviceHandle: THandle; ExOnOff : Integer ): Integer; stdcall; external 'FbiDa.DLL';
function DaGetRelayStatus(hDeviceHandle: THandle; var Status : Integer ): Integer; stdcall; external 'FbiDa.DLL';
function DaGetOVStatus(hDeviceHandle: THandle; var LowStatus : Integer; var HighStatus : Integer ): Integer; stdcall; external 'FbiDa.DLL';

var
  ghDeviceHandle: THandle;
  ghChannel: Array [0..1] of DWORD;
  OpenFlag: Integer;
  gConfig: DASMPLREQ;
  gInfo: DABOARDSPEC;

implementation



end.
