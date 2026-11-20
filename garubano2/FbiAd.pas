unit FbiAd;

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
//     GPC/GPF-3100 Overlapped Process Identifier
//
//-----------------------------------------------------------------------------------------------
const
  FLAG_SYNC 	= 1;	// Sampling as an non-overlapped operation
  FLAG_ASYNC	= 2;	// Sampling as overlapped operation


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 File Format Identifier
//
//-----------------------------------------------------------------------------------------------
const
  FLAG_BIN	= 1;	// Binary format file
  FLAG_CSV	= 2;	// CSV format file


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Sampling Status Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_STATUS_STOP_SAMPLING  = 1;	   // The sampling is stopped.
  AD_STATUS_WAIT_TRIGGER   = 2;	   // The sampling is waiting for a trigger.
  AD_STATUS_NOW_SAMPLING   = 3;	   // The sampling is running.


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Event Factor Edentifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_EVENT_SMPLNUM         = 1;    // The specified number of samples has been acquired. 
  AD_EVENT_STOP_TRIGGER	   = 2;    // The sampling has been stopped because a trigger is asserted.
  AD_EVENT_STOP_FUNCTION   = 3;    // The sampling has been stopped by software.
  AD_EVENT_STOP_TIMEOUT    = 4;    // The sampling has been stopped because a time-out interval elapsed. 
  AD_EVENT_STOP_SAMPLING   = 5;    // The sampling is stopped.
  AD_EVENT_STOP_SCER       = 6;    // The sampling is stopped by a clock error.
  AD_EVENT_STOP_ORER       = 7;    // The sampling is stopped by an overrun error.
  AD_EVENT_SCER            = 8;    // The sampling pacer clock error is occurred.
  AD_EVENT_ORER            = 9;    // The overrun error is occurred.
  AD_EVENT_STOP_LV_1       = 10;   // The channel 1 sampling is stopped. (Only applicable to the PCI-3179)
  AD_EVENT_STOP_LV_2       = 11;   // The channel 2 sampling is stopped. (Only applicable to the PCI-3179)
  AD_EVENT_STOP_LV_3       = 12;   // The channel 3 sampling is stopped. (Only applicable to the PCI-3179)
  AD_EVENT_STOP_LV_4       = 13;   // The channel 4 sampling is stopped. (Only applicable to the PCI-3179)
  AD_EVENT_RANGE           = 14;   // The AD conversion value reached the full-scale range.
  AD_EVENT_STOP_RANGE      = 15;   // The sampling is stopped by the full-scale range detection.
  AD_EVENT_OVPM            = 16;   // The AD conversion value reached the OVPM.
  AD_EVENT_STOP_OVPM       = 17;   // The sampling is stopped by the OVPM detection.

//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Input Configuration Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_INPUT_SINGLE	   = 1;	   // Single-ended input
  AD_INPUT_DIFF		   = 2;	   // Differential input


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Volume Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_ADJUST_BIOFFSET   = 1;	   // Bipolar offset
  AD_ADJUST_UNIOFFSET  = 2;	   // Unipolar offset
  AD_ADJUST_BIGAIN     = 3;	   // Bipolar gain
  AD_ADJUST_UNIGAIN    = 4;    // Unipolar gain


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Calibration Item Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_ADJUST_UP         = 1;	   // Increases the volume.
  AD_ADJUST_DOWN       = 2;	   // Decreases the volumde.
  AD_ADJUST_STORE      = 3;	   // Saves the present value to the non-volatile memory.
  AD_ADJUST_STANDBY    = 4;	   // Places the electronic volume device into the standby mode.
  AD_ADJUST_NOT_STORE  = 5;    // Not save the value.

//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Read Adjust Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_ADJUST_READ_FACTORY = 1;	// Factrory Setting
  AD_ADJUST_READ_USER = 2;		// User Setting


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Data Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_DATA_PHYSICAL	   = 1;	   // Physical value (voltage [V] or current [mA])
  AD_DATA_BIN8		   = 2;	   // 8-bit binary
  AD_DATA_BIN12		   = 3;	   // 12-bit binary
  AD_DATA_BIN16		   = 4;	   // 16-bit binary
  AD_DATA_BIN24		   = 5;	   // 24-bit binary
  AD_DATA_BIN10		   = 6;	   // 10-bit binary

//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Data Conversion Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_CONV_SMOOTH	   = 1;	   // Converts the data with interpolation.
  AD_CONV_AVERAGE1	   = $100; // Converts the data with the simple averaging.
  AD_CONV_AVERAGE2	   = $200; // Converts the data with the shifted averaging.


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Sampling Mode Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_IO_SAMPLING	   = 1;	   // Programmed I/O
  AD_FIFO_SAMPLING	   = 2;	   // FIFO
  AD_MEM_SAMPLING	   = 4;	   // Memory
  AD_BM_SAMPLING       = 8;    // Bus master


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Trigger Timing Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_TRIG_START		   = 1;	   // Start-trigger (default setting)
  AD_TRIG_STOP		   = 2;	   // Stop-trigger
  AD_TRIG_START_STOP   = 3;	   // Start/stop-trigger


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Trigger Level Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_FREERUN		   = 1;        // No trigger (default setting)
  AD_EXTTRG            = 2;        // External trigger
  AD_EXTTRG_DI		   = 3;        // External trigger with mask using general purpose digital input pin
  AD_LEVEL_P		   = 4;        // Analog trigger (low-to-high transition)
  AD_LEVEL_M		   = 5;        // Analog trigger (high-to-low transition)
  AD_LEVEL_D		   = 6;        // Analog trigger (low-to-high or high-to-low transition)
  AD_INRANGE		   = 7;        // Analog trigger (into the range)
  AD_OUTRANGE		   = 8;	       // Analog trigger (out of the range)
  AD_ETERNITY		   = 9;    	   // Infinite sampling
  AD_SMPLNUM           = 10;       // Specified number
  AD_START_SIGTIMER    = 11;       // Interval timer
  AD_START_DA_START    = 12;       // Analog output start (DaStartSampling)
  AD_START_DA_STOP     = 13;       // Analog output stop
  AD_START_DA_IO       = 14;       // Analog output (DaOutputDA)
  AD_START_DA_SMPLNUM  = 15;       // Analog output number
  AD_STOP_DA_START     = 12;       // Analog output start (DaStartSampling)
  AD_STOP_DA_STOP      = 13;       // Analog output stop
  AD_STOP_DA_IO        = 14;       // Analog output (DaOutputDA)
  AD_STOP_DA_SMPLNUM   = 15;       // Analog output number
  AD_START_P1	   = $00000010;	   // Start-trigger (Level 1): low-to- high transition
  AD_START_M1	   = $00000020;	   // Start-trigger (Level 1): high-to-low transition
  AD_START_D1	   = $00000040;	   // Start-trigger (Level 1): high-to-low or low-to-high transition (direction DON'T CARE)
  AD_START_P2	   = $00000080;	   // Start-trigger (Level 2): low-to- high transition
  AD_START_M2	   = $00000100;	   // Start-trigger (Level 2): high-to-low transition
  AD_START_D2	   = $00000200;	   // Start-trigger (Level 2): high-to-low or low-to-high transition (direction DON'T CARE)
  AD_STOP_P1	   = $00000400;	   // Stop-trigger (Level 1): low-to-high transition
  AD_STOP_M1	   = $00000800;    // Stop-trigger (Level 1): high-to-low transition
  AD_STOP_D1	   = $00001000;    // Stop-trigger (Level 1): high-to-low or low-to-high transition (direction DON'T CARE)
  AD_STOP_P2	   = $00002000;	   // Stop-trigger (Level 2): low-to-high transition
  AD_STOP_M2	   = $00004000;	   // Stop-trigger (Level 2): high-to-low transition
  AD_STOP_D2	   = $00008000;	   // Stop-trigger (Level 2): high-to-low or low-to-high transition (direction DON'T CARE)
  AD_ANALOG_FILTER = $00010000;	   // Uses an analog trigger filter.
  AD_START_CNT_EQ  = $00020000;	   // Start-trigger: Counter equal
  AD_STOP_CNT_EQ   = $00040000;	   // Stop-trigger: Counter equal
  AD_START_DI_EQ   = $00080000;    // Stop-trigger: DI equal
  AD_STOP_DI_EQ    = $00100000;    // Stop-trigger: DI equal
  AD_STOP_SOFT     = $00200000;	   // Stop-trigger: Soft stop
  AD_START_Z_CLR   = $00400000;	   // Start-trigger: Z clear
  AD_STOP_Z_CLR    = $00800000;    // Stop-trigger: Z clear
  AD_START_SYNC1   = $10000000;    // Start-trigger: Internal Sync1 trigger
  AD_START_SYNC2   = $20000000;    // Start-trigger: Internal Sync2 trigger
  AD_STOP_SYNC1    = $40000000;    // Stop-trigger: Internal Sync1 trigger
  AD_STOP_SYNC2    = $80000000;    // Stop-trigger: Internal Sync2 trigger

//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Polarity Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_DOWN_EDGE = 1;	         // Falling edge (default setting)
  AD_UP_EDGE   = 2;	         // Rising edge
  AD_EXTRG_IN  = 3;          // External trigger input
  AD_EXCLK_IN  = 4;          // External clock input

  AD_EDGE_P1   = $0010;      // Level 1: low-to-high transition
  AD_EDGE_M1   = $0020;      // Level 1: high-to-low transition
  AD_EDGE_D1   = $0040;      // Level 1: high-to-low or low-to-high transition (direction DON'T CARE)
  AD_EDGE_P2   = $0080;      // Level 2: low-to-high transition
  AD_EDGE_M2   = $0100;      // Level 2: high-to-low transition
  AD_EDGE_D2   = $0200;      // Level 2: high-to-low or low-to-high transition (direction DON'T CARE)
  AD_DISABLE   = $80000000;  // No pulse output (default setting)

  AD_TRIG_MODE = 2;          //
  AD_BUSY_MODE = 3;          //
  AD_POST_MODE = 4;          //
  AD_ENABLE    = 5;          //
  AD_SMP1_MODE = 6;          //
  AD_SMP2_MODE = 7;          //
  AD_ATRIG_MODE = 8;          //
  
//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Pulse Polarity Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_LOW_PULSE		   = 1;	   // Negative pulse (default setting)
  AD_HIGH_PULSE		   = 2;	   // Positive pulse


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Double-Clocked Mode Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_NORMAL_MODE	   = 1;	   // Normal mode (default setting)
  AD_FAST_MODE		   = 2;	   // Double-clocked mode

//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Status Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_NO_STATUS	           = 1;    // Adds no bus master sampling status. (default setting)
  AD_ADD_STATUS            = 2;    // Adds bus master sampling status.


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Error Control Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_STOP_SCER	           = 2;    // Stops sampling by a sampling clock error.
  AD_STOP_ORER             = 4;    // Stops sampling by an overrun error.


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Data Save Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_APPEND                = 1;    // Adds new data at the end of the buffer. (default setting)
  AD_OVERWRITE             = 2;    // Overwrites new data on existing data.


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Range Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_0_1V		   = $00000001;		// Voltage: unipolar 0 V to 1 V
  AD_0_2P5V		   = $00000002;		// Voltage: unipolar 0 V to 2.5 V
  AD_0_5V		   = $00000004;		// Voltage: unipolar 0 V to 5 V
  AD_0_10V		   = $00000008;		// Voltage: unipolar 0 V to 10 V
  AD_1_5V		   = $00000010;		// Voltage: unipolar 1 V to 5 V
  AD_0_2V		   = $00000020;		// Voltage: unipolar 0 V to 2 V
  AD_0_0P125V	   = $00000040;		// Voltage: unipolar 0 V to 0.125 V
  AD_0_1P25V	   = $00000080;		// Voltage: unipolar 0 V to 1.25 V
  AD_0_0P625V	   = $00000100;		// Voltage: unipolar 0 V to 0.625 V
  AD_0_0P156V	   = $00000200;		// Voltage: unipolar 0 V to 0.15625 V
  AD_0_20mA		   = $00001000;		// Current: unipolar 0 mA to 20 mA
  AD_4_20mA		   = $00002000;		// Current: unipolar 4 mA to 20 mA
  AD_20mA		   = $00004000;		// Current: bipolar +/- 20 mA
  AD_1V			   = $00010000;		// Voltage: bipolar +/- 1 V
  AD_2P5V		   = $00020000;		// Voltage: bipolar +/- 2.5 V
  AD_5V			   = $00040000;		// Voltage: bipolar +/- 5 V
  AD_10V		   = $00080000;		// Voltage: bipolar +/- 10 V
  AD_20V		   = $00100000;		// Voltage: bipolar +/- 20 V
  AD_50V		   = $00200000;		// Voltage: bipolar +/- 50 V
  AD_0P125V		   = $00400000;		// Voltage: bipolar +/- 0.125 V
  AD_1P25V		   = $00800000;		// Voltage: bipolar +/- 1.25 V
  AD_0P625V		   = $01000000;		// Voltage: bipolar +/- 0.625 V
  AD_0P156V		   = $02000000;		// Voltage: bipolar +/- 0.15625 V
  AD_1P25V_AC	   = $04000000;		// Voltage: bipolar +/- 1.25 V (AC-Coupling)
  AD_0P625V_AC	   = $08000000;		// Voltage: bipolar +/- 0.625 V (AC-Coupling)
  AD_0P156V_AC	   = $10000000;		// Voltage: bipolar +/- 0.15625 V (AC-Coupling)
  AD_AC_COUPLING   = $40000000;		// AC Coupling
  AD_GND		   = $80000000;		// GND

//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Isolation Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_ISOLATION		   = 1;	   // Isolated
  AD_NOT_ISOLATION	   = 2;	   // Not isolated


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Synchronous Mode Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_MASTER_MODE           = 1;     // Master mode
  AD_SLAVE_MODE            = 2;     // Slave mode

//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Synchronous Number Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_SYNC_NUM_1	= $0100;
  AD_SYNC_NUM_2	= $0200;
  AD_SYNC_NUM_3	= $0400;
  AD_SYNC_NUM_4	= $0800;
  AD_SYNC_NUM_5	= $1000;
  AD_SYNC_NUM_6	= $2000;
  AD_SYNC_NUM_7	= $4000;

//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Calibration Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_SELF_CALIBRATION      = 1;     // Self calibration of the AD converter
  AD_ZEROSCALE_CALIBRATION = 2;     // Zero voltage calibration (system calibration)
  AD_FULLSCALE_CALIBRATION = 3;     // Full scale voltage calibration (system calibration)


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Full-scale Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_STATUS_NEGATIVE_FULL_SCALE = 1;    // Negative full-scale
  AD_STATUS_POSITIVE_FULL_SCALE = 2;    // Positive full-scale
  AD_STATUS_UNDER_RANGE         = 1;    // Negative full-scale
  AD_STATUS_OVER_RANGE          = 2;    // Positive full-scale


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 OVPM Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_STATUS_OVPM_NORMAL = 0;		// NormalRange
  AD_STATUS_OVPM_HIGH_RANGE = 1;	// Voltage: bipolar +/- 50 V Range
  AD_STATUS_OVPM_GND_RANGE = 2;		// GND


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 PCI-3525 CN3,4 Function Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_CN_FREE       = 0;               // Not used
  AD_CN_EXTRG_IN   = 1;               // External trigger input
  AD_CN_EXTRG_OUT  = 2;               // External trigger output
  AD_CN_EXCLK_IN   = 3;               // External clock input
  AD_CN_EXCLK_OUT  = 4;               // External clock output
  AD_CN_EXINT_IN   = 5;               // External interrupt input
  AD_CN_ATRG_OUT   = 6;               // Analog trigger out
  AD_CN_DI         = 7;               // Digital input
  AD_CN_DO         = 8;               // Digital output
  AD_CN_DAOUT      = 9;               // Analog output
  AD_CN_OPEN       = 10;              // Open
  AD_CN_EXSMP1_OUT = 12;              // Sampling Status1
  AD_CN_EXSMP2_OUT = 13;              // Sampling Status2
  AD_CN_DIO        = 1;               // DIO
  AD_CN_CONTROL    = 2;               // Control used
  AD_CN_CNT        = 3;               // Counter used

//-----------------------------------------------------------------------------------------------
//
//		GPC/GPF-3100 CPZ-360810 DIN/DOUT Function Identifier
//
//-----------------------------------------------------------------------------------------------
const
 AD_EX_DIO1 = 1;						// DIN/DOUT1
 AD_EX_DIO2 = 2;						// DIN/DOUT2
 AD_EX_DIO3 = 3;						// DIN/DOUT3
 AD_EX_DIO4 = 4;						// DIN/DOUT4
 AD_EX_DIO5 = 5;						// DIN/DOUT5
 AD_EX_DIO6 = 6;						// DIN/DOUT6
 AD_EX_DIO7 = 7;						// DIN/DOUT7
 AD_EX_DIO8 = 8;						// DIN/DOUT8

//-----------------------------------------------------------------------------------------------
//
//		GPC/GPF-3100 Degital Filter Identifier
//
//-----------------------------------------------------------------------------------------------
const
 AD_DF_8   = 0;    // 8 (default setting)
 AD_DF_16  = 1;	   // 16
 AD_DF_32  = 2;	   // 32
 AD_DF_64  = 3;	   // 64
 AD_DF_128 = 4;	   // 128
 AD_DF_256 = 5;	   // 256


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Measure Temperature
//
//-----------------------------------------------------------------------------------------------
const
 AD_GET_CURRENT_TEMPERATURE = 1;			// Current Temperature
 AD_LOAD_TEMPERATURE = 2;					// User area Temperature
 AD_LOAD_FACTORY_SETTING_TEMPERATURE = 3;	// Factory setting Temperature
 AD_SAVE_TEMPERATURE_USER = 4;				// User area of Eeprom 


//-----------------------------------------------------------------------------------------------
//
//     GPC/GPF-3100 Error Identifier
//
//-----------------------------------------------------------------------------------------------
const
  AD_ERROR_SUCCESS						= $00000000;
  AD_ERROR_NOT_DEVICE					= $C0000001;
  AD_ERROR_NOT_OPEN						= $C0000002;
  AD_ERROR_INVALID_HANDLE				= $C0000003;
  AD_ERROR_ALREADY_OPEN					= $C0000004;
  AD_ERROR_NOT_SUPPORTED				= $C0000009;
  AD_ERROR_NOW_SAMPLING					= $C0001001;
  AD_ERROR_STOP_SAMPLING				= $C0001002;
  AD_ERROR_START_SAMPLING				= $C0001003;
  AD_ERROR_SAMPLING_TIMEOUT				= $C0001004;
  AD_ERROR_INVALID_PARAMETER	        = $C0001021;
  AD_ERROR_ILLEGAL_PARAMETER	        = $C0001022;
  AD_ERROR_NULL_POINTER					= $C0001023;
  AD_ERROR_GET_DATA						= $C0001024;
  AD_ERROR_USED_DA                      = $C0001025;
  AD_ERROR_FILE_OPEN		        	= $C0001041;
  AD_ERROR_FILE_CLOSE		    	    = $C0001042;
  AD_ERROR_FILE_READ					= $C0001043;
  AD_ERROR_FILE_WRITE		        	= $C0001044;
  AD_ERROR_INVALID_DATA_FORMAT			= $C0001061;
  AD_ERROR_INVALID_AVERAGE_OR_SMOOTHING	= $C0001062;
  AD_ERROR_INVALID_SOURCE_DATA	        = $C0001063;
  AD_ERROR_NOT_ALLOCATE_MEMORY	        = $C0001081;
  AD_ERROR_NOT_LOAD_DLL					= $C0001082;
  AD_ERROR_CALL_DLL						= $C0001083;


// -----------------------------------------------------------------------
//
//     GPC/GPF-3100 Structure Declaration
//
// -----------------------------------------------------------------------
//
//     Sampling Condition Structure for Each Channel
//
// -----------------------------------------------------------------------
type
  ADSMPLCHREQ = record
    ulChNo : Cardinal;
    ulRange : Cardinal;
  end;


// -----------------------------------------------------------------------
//
//     Analog Trigger Condition Structure for Each Channel
//
// -----------------------------------------------------------------------
type
  ADTRIGCHREQ = record
    ulChNo : Cardinal;
    fTrigLevel : Single;
    fHysteresis : Single;
  end;


// -----------------------------------------------------------------------
//
//     Sampling Condition Structure
//
// -----------------------------------------------------------------------
type
  ADSMPLREQ = record
    ulChCount : Cardinal;
    SmplChReq : array[0..255] of ADSMPLCHREQ;
    ulSamplingMode : Cardinal;
    ulSingleDiff : Cardinal;
    ulSmplNum : Cardinal;
    ulSmplEventNum : Cardinal;
    fSmplFreq : Single;
    ulTrigPoint : Cardinal;
    ulTrigMode : Cardinal;
    lTrigDelay : Integer;
    ulTrigCh : Cardinal;
    fTrigLevel1 : Single;
    fTrigLevel2 : Single;
    ulEClkEdge : Cardinal;
    ulATrgPulse : Cardinal;
    ulTrigEdge : Cardinal;
    ulTrigDI : Cardinal;
    ulFastMode : Cardinal;
  end;


// -----------------------------------------------------------------------
//
//     Sampling Condition Structure for Bus Master
//
// -----------------------------------------------------------------------
type
  ADBMSMPLREQ = record
    ulChCount : Cardinal;
    SmplChReq : array[0..255] of ADSMPLCHREQ;
    ulSingleDiff : Cardinal;
    ulSmplNum : Cardinal;
    ulSmplEventNum : Cardinal;
    ulSmplRepeat : Cardinal;
    ulBufferMode : Cardinal;
    fSmplFreq : Single;
    fScanFreq : Single;
    ulStartMode : Cardinal;
    ulStopMode : Cardinal;
    ulPreTrigDelay : Cardinal;
    ulPostTrigDelay : Cardinal;
    TrigChReq : array[0..1] of ADTRIGCHREQ;
    ulATrgMode : Cardinal;
    ulATrgPulse : Cardinal;
    ulStartTrigEdge : Cardinal;
    ulStopTrigEdge : Cardinal;
    ulTrigDI : Cardinal;
    ulEClkEdge : Cardinal;
    ulFastMode : Cardinal;
    ulStatusMode : Cardinal;
    ulErrCtrl : Cardinal;
  end;

// -----------------------------------------------------------------------
//
//     Sampling Condition Structure for Memory
//
// -----------------------------------------------------------------------
type
  ADMEMSMPLREQ = record
	ulChCount : Cardinal;
    SmplChReq : array[0..255] of ADSMPLCHREQ;
	ulSingleDiff : Cardinal;
    fSmplFreq : Single;
	ulStopMode : Cardinal;
	ulPreTrigDelay : Cardinal;
	ulPostTrigDelay : Cardinal;
    TrigChReq : array[0..1] of ADTRIGCHREQ;
    ulATrgMode : Cardinal;
    ulATrgPulse : Cardinal;
	ulStopTrigEdge : Cardinal;
	ulEClkEdge : Cardinal;
	ulFastMode : Cardinal;
	ulStatusMode : Cardinal;
	ulErrCtrl : Cardinal;
  end;

// -----------------------------------------------------------------------
//
//     Board Specification Structure
//
// -----------------------------------------------------------------------
type
  ADBOARDSPEC = record
    ulBoardType : Cardinal;
    ulBoardID : Cardinal;
    dwSamplingMode : DWORD;
    ulChCountS : Cardinal;
    ulChCountD : Cardinal;
    ulResolution : Cardinal;
    dwRange : DWORD;
    ulIsolation : Cardinal;
    ulDi : Cardinal;
    ulDo : Cardinal;
  end;

// -----------------------------------------------------------------------
//
//     GPC/GPF-3100 Function Declaration
//
// -----------------------------------------------------------------------

function  AdOpen(lpszName: Pointer): THandle; stdcall; external 'FbiAd.dll';
function  AdClose(hDeviceHandle: THandle): DWORD; stdcall; external 'FbiAd.dll';

function  AdGetDeviceInfo(hDeviceHandle: THandle; var pAdBoardSpec: ADBOARDSPEC): DWORD; stdcall; external 'FbiAd.dll';

function  AdSetBoardConfig( hDeviceHandle: THandle;  hEvent: THandle; lpCallBackProc: FARPROC;   dwUser: DWORD ): DWORD; stdcall; external 'FbiAd.dll';
function  AdGetBoardConfig( hDeviceHandle: THandle;  var ulAdSmplEventFactor: Cardinal ): DWORD; stdcall; external 'FbiAd.dll';

function  AdSetSamplingConfig(hDeviceHandle: THandle; var pAdSmplConfig: ADSMPLREQ): DWORD; stdcall; external 'FbiAd.dll';
function  AdGetSamplingConfig(hDeviceHandle: THandle; var pAdSmplConfig: ADSMPLREQ): DWORD; stdcall; external 'FbiAd.dll';

function  AdGetSamplingData(hDeviceHandle: THandle; pSmplData: Pointer; var ulSmplNum: Cardinal): DWORD; stdcall; external 'FbiAd.dll';
function  AdClearSamplingData(hDeviceHandle: THandle): DWORD; stdcall; external 'FbiAd.dll';

function  AdStartSampling(hDeviceHandle: THandle; ulSyncFlag: Cardinal): DWORD; stdcall; external 'FbiAd.dll';
function  AdStartFileSampling(hDeviceHandle: THandle; pszPathName: Pointer; ulFileFlag: Cardinal): DWORD; stdcall; external 'FbiAd.dll';
function  AdTriggerSampling(hDeviceHandle: THandle; ulChNo: Integer; ulRange: Integer; ulSingleDiff: Integer; ulTriggerMode: Integer; ulTrigEdge: Integer; ulSmplNum: Integer): DWORD; stdcall; external 'FbiAd.DLL';
function  AdMemTriggerSampling(hDeviceHandle: THandle; ulChCount: Integer; var lpSmplChReq: ADSMPLCHREQ; ulSmplNum: Integer; ulRepeatCount: Integer; ulTrigEdge: Integer; fSmplFreq: Single; ulEClkEdge: Integer; ulFastMode: Integer): DWORD; stdcall; external 'FbiAd.DLL';
function  AdSyncSampling(hDeviceHandle: THandle; ulMode: Integer): DWORD; stdcall; external 'FbiAd.dll';

function  AdStopSampling(hDeviceHandle: THandle): DWORD; stdcall; external 'FbiAd.dll';

function  AdGetStatus(hDeviceHandle: THandle; var ulAdSmplStatus: Cardinal; var ulAdSmplCount: Cardinal; var ulAdAvailCount: Cardinal): DWORD; stdcall; external 'FbiAd.dll';

function  AdInputAD(hDeviceHandle: THandle; ulCh: Cardinal; ulSingleDiff: Cardinal; var lpAdSmplChReq: ADSMPLCHREQ; lpData: Pointer): DWORD; stdcall; external 'FbiAd.dll';

function  AdInputDI(hDeviceHandle: THandle; var dwData: DWORD): DWORD; stdcall; external 'FbiAd.dll';
function  AdOutputDO(hDeviceHandle: THandle; dwData: DWORD): DWORD; stdcall; external 'FbiAd.dll';

function  AdAdjustVR(hDeviceHandle: THandle; ulAdjustCh: Cardinal; ulSingleDiff: Cardinal; ulSelVolume: Cardinal; ulControl: Cardinal; ulTap: Cardinal): DWORD; stdcall; external 'FbiAd.dll';
function  AdReadAdjustVR(hDeviceHandle: THandle; ulAdjustCh: Cardinal): DWORD; stdcall; external 'FbiAd.dll';
function  AdReadAdjustVREx(hDeviceHandle: THandle; ulAdjustCh: Cardinal; ulControl: Cardinal): DWORD; stdcall; external 'FbiAd.dll';

function  AdDataConv(uSrcFormCode: UINT; pSrcData: Pointer; uSrcSmplDataNum: DWORD; var pSrcSmplReq: ADSMPLREQ; uDestFormCode: UINT; pDestData: Pointer; var puDestSmplDataNum: DWORD; var pDestSmplReq: ADSMPLREQ; uEffect: UINT; uCount: UINT; lpfnConv: Pointer): DWORD; stdcall; external 'FbiAdDC.dll';
function  AdReadFile(pszPathName: Pointer; pSmplData: Pointer; ulFormCode: Cardinal): DWORD; stdcall; external 'FbiAdDC.dll';

function  AdBmSetSamplingConfig(hDeviceHandle: THandle; var pAdBmSmplConfig: ADBMSMPLREQ): DWORD; stdcall; external 'FbiAd.dll';
function  AdBmGetSamplingConfig(hDeviceHandle: THandle; var pAdBmSmplConfig: ADBMSMPLREQ): DWORD; stdcall; external 'FbiAd.dll';
function  AdBmGetSamplingData(hDeviceHandle: THandle; pBmSmplData: Pointer; var ulBmSmplNum: Cardinal): DWORD; stdcall; external 'FbiAd.dll';
function  AdBmStartFileSampling(hDeviceHandle: THandle; pszPathName : Pointer; ulFileFlag : Cardinal) : DWORD; stdcall; external 'FbiAd.dll';

function  AdLvSetSamplingConfig(hDeviceHandle: THandle; ulChNo: DWORD; ulSmplNum: DWORD; fSmplFreq:Single; ulRange: DWORD; hEvent: THandle; lpCallBackProc: FARPROC; dwUser: DWORD): DWORD; stdcall; external 'FbiAd.DLL';
function  AdLvGetSamplingConfig(hDeviceHandle: THandle; ulChNo: DWORD; var ulSmplNum: DWORD; var fSmplFreq:Single; var ulRange: DWORD): DWORD; stdcall; external 'FbiAd.DLL';
function  AdLvCalibration(hDeviceHandle: THandle; ulChNo: DWORD; ulCalibration: DWORD): DWORD; stdcall; external 'FbiAd.DLL';
function  AdLvGetSamplingData(hDeviceHandle : THandle; ulChNo: DWORD; pSmplData : Pointer; var ulSmplNum : Cardinal): DWORD; stdcall; external 'FbiAd.DLL';
function  AdLvStartSampling(hDeviceHandle: THandle; ulChNo: Cardinal): DWORD; stdcall; external 'FbiAd.DLL';
function  AdLvStopSampling(hDeviceHandle: THandle; ulChNo: DWORD): DWORD; stdcall; external 'FbiAd.DLL';
function  AdLvGetStatus(hDeviceHandle: THandle; ulChNo : Cardinal; var ulAdSmplStatus : Cardinal; var ulAdSmplCount : Cardinal; var ulAdAvailCount : Cardinal): DWORD; stdcall; external 'FbiAd.DLL';
function  AdMeasureTemperature(hDeviceHandle: THandle; var fTemperature : Single): DWORD; stdcall; external 'FbiAd.DLL';
function  AdMeasureTemperatureEx(hDeviceHandle: THandle; var fTemperature : Single; ulControl : Cardinal): DWORD; stdcall; external 'FbiAd.DLL';

function  AdAllocateSamplingBuffer(hDeviceHandle: THandle): DWORD; stdcall; external 'FbiAd.DLL';
function  AdReadSamplingBuffer(hDeviceHandle: THandle; lOffset: Cardinal; var ulSmplNum : Cardinal; pSmplData : Pointer): DWORD; stdcall; external 'FbiAd.DLL';

function  AdSetRangeEvent(hDeviceHandle : THandle; dwEventMask : DWORD; dwStopMode : DWORD) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdGetRangeEventStatus(hDeviceHandle: THandle; var ulEventChNo : Cardinal; var ulEventStatus : Cardinal ) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdResetRangeEvent(hDeviceHandle: THandle) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdGetOverRangeChStatus(hDeviceHandle: THandle; var ulEventChNo : Cardinal ) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdResetOverRangeCh(hDeviceHandle: THandle) : DWORD; stdcall; external 'FbiAd.DLL';

function  AdSetInterval(hDeviceHandle: THandle; ulInterval : Cardinal ) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdGetInterval(hDeviceHandle: THandle; var ulInterval : Cardinal ) : DWORD; stdcall; external 'FbiAd.DLL';

function  AdSetFunction(hDeviceHandle: THandle; ulChNo : Cardinal; ulFunction : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdGetFunction(hDeviceHandle: THandle; ulChNo : Cardinal; var ulFunction : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';

function  AdSetFilter(hDeviceHandle: THandle; ulFilter : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdGetFilter(hDeviceHandle: THandle; var ulFilter : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';

function  AdSetOutMode(hDeviceHandle: THandle; ulExTrgMode : Cardinal; ulExClkMode : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdGetOutMode(hDeviceHandle: THandle; var ulExTrgMode : Cardinal; var ulExClkMode : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';

function  AdMemSetSamplingConfig(hDeviceHandle: THandle; var pAdMemSmplConfig: ADMEMSMPLREQ) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdMemGetSamplingConfig(hDeviceHandle: THandle; var pAdMemSmplConfig: ADMEMSMPLREQ) : DWORD; stdcall; external 'FbiAd.DLL';

function  AdMemSetDiPattern(HhDeviceHandle: THandle; ulCh : Cardinal; var ulPatternTrig : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdMemGetDiPattern(hDeviceHandle: THandle; var ulPatternTrig : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';

function  AdCommonGetPciDeviceInfo(hDeviceHandle : THandle; var dwDeviceID : Cardinal; var dwVendorID : Cardinal; var dwClassCode : Cardinal; var dwRevisionID : Cardinal; var dwBaseAddress0 : Cardinal; var dwBaseAddress1 : Cardinal; var dwBaseAddress2 : Cardinal; var dwBaseAddress3 : Cardinal; var dwBaseAddress4 : Cardinal; var dwBaseAddress5 : Cardinal; var dwSubsystemID : Cardinal; var dwSubsystemVendorID : Cardinal; var dwInterruptLine : Cardinal; var dwBoardID : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdFifoGetSamplingData(hDeviceHandle: THandle; pSmplData: Pointer; pDiData: Pointer; var ulSmplNum: Cardinal): DWORD; stdcall; external 'FbiAd.dll';

function  AdBmStartSampling(hDeviceHandle: THandle; pSmplData: Pointer; ulSize: Cardinal): DWORD; stdcall; external 'FbiAd.dll';

function  AdCalibration(hDeviceHandle: THandle; ulChannel : Cardinal; ulRange : Cardinal; ulSingleDiff : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdOutputSync(hDeviceHandle: THandle; TrgLine : Cardinal; ulExClkMode : Cardinal; TrgMode : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';

function  AdSetDisConnection(hDeviceHandle: THandle; dwChannel : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdGetDisConnection(hDeviceHandle: THandle; var dwChannel : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';
function  AdGetDisConnectionStatus(hDeviceHandle: THandle; var dwStatus : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';

function  AdSetEndEventConfig(hDeviceHandle: THandle; Mode : Cardinal) : DWORD; stdcall; external 'FbiAd.DLL';

var
  ghDeviceHandle: THandle;
  ghChannel: Array[0..1] of DWORD;
  OpenFlag: Integer;
  gConfig: ADSMPLREQ;
  gBmConfig: ADBMSMPLREQ;
  gMemConfig: ADMEMSMPLREQ;
  gInfo: ADBOARDSPEC;

implementation


end.
