/*
 *	PDCVALUE.h
 *	Photron Device Control SDK
 *	Value Definition
 *
 *	Copyright (C) 2006-2007 PHOTRON LIMITED
 */

#ifndef	_PDC_VALUE_H_
#define	_PDC_VALUE_H_

/*	General	*/
#define	PDC_MAX_DEVICE			4		/*	Maximum connection devices	*/
#define	PDC_MAX_INTERFACE_TYPE	10		/*	Maximum interface type	*/
#define	PDC_MAX_DEVICE_TYPE		64		/*	Maximum device type	*/
#define	PDC_MAX_CHILD_DEVICE	4		/*	Maximum child devices	*/
#define	PDC_MAX_PARTITION		64		/*	Maximum partitions	*/
#define	PDC_MAX_LIST_NUMBER		256		/*	Maximum list parameters	*/
#define	PDC_MAX_STRING_LENGTH	256		/*	Maximum strings	*/
#define	PDC_MAX_LUTLIST_NUMBER	4096	/*	Maximum LUT list parameters	*/

/*	Function On/Off(General)	*/
#define	PDC_FUNCTION_OFF					0
#define	PDC_FUNCTION_ON						1

/*	Function presence	*/
#define	PDC_EXIST_NOTSUPPORTED				0
#define	PDC_EXIST_SUPPORTED					1

#define	PDC_EXIST_SENSOR_GAIN				2
#define	PDC_EXIST_SENSOR_GAMMA				3
#define	PDC_EXIST_COLORTEMP					4
#define	PDC_EXIST_DSSHUTTER					5
#define	PDC_EXIST_LUT						6
#define	PDC_EXIST_SHADING					7
#define	PDC_EXIST_II						8
#define	PDC_EXIST_EDGE_ENHANCE				9
#define	PDC_EXIST_VIDEO_OUT					10
#define	PDC_EXIST_SHUTTERLOCK				11
#define	PDC_EXIST_PARTITIONINC				12
#define	PDC_EXIST_HEADEXCHANGE				13
#define	PDC_EXIST_KEYLOCK					14
#define	PDC_EXIST_IRIG						15
#define	PDC_EXIST_MCDL						16
#define	PDC_EXIST_FRAME_ZERO				17
#define	PDC_EXIST_PIXELGAIN					18
#define	PDC_EXIST_COLOR_ENHANCE				19
#define	PDC_EXIST_AUTO_EXPOSURE				20
#define	PDC_EXIST_DELAY						21
#define	PDC_EXIST_DOWNLOAD_MODE				22
#define	PDC_EXIST_VARIABLE_FUNCTION			23
#define	PDC_EXIST_LOWLIGHT_MODE				24
#define	PDC_EXIST_PROGSWITCH_MODE			25
#define	PDC_EXIST_SHUTTERSPEED_USEC			26
#define	PDC_EXIST_SYNCOUT_TIMES				27
#define	PDC_EXIST_STORE						28
#define	PDC_EXIST_ETHER_INFO				29
#define	PDC_EXIST_IRIG_PHASELOCK			30
#define	PDC_EXIST_HIGH_SPEED_MODE			31
#define	PDC_EXIST_BURST_TRANSFER			32
#define	PDC_EXIST_STEP_SHUTTER				33
#define PDC_EXIST_LIVE_RESOLUTION			34

/*	Color / Monochrome	*/
#define	PDC_COLORTYPE_MONO					0
#define	PDC_COLORTYPE_COLOR					1

/*	Status	*/
#define	PDC_STATUS_LIVE						0x00
#define	PDC_STATUS_PLAYBACK					0x01
#define	PDC_STATUS_RECREADY					0x02
#define	PDC_STATUS_ENDLESS					0x04
#define	PDC_STATUS_REC						0x08
#define	PDC_STATUS_SAVE						0x10
#define	PDC_STATUS_LOAD						0x20

/*	Trigger mode	*/
#define	PDC_TRIGGER_START					(0x00 << 24)
#define	PDC_TRIGGER_CENTER					(0x01 << 24)
#define	PDC_TRIGGER_END						(0x02 << 24)
#define	PDC_TRIGGER_RANDOM					(0x03 << 24)
#define	PDC_TRIGGER_MANUAL					(0x04 << 24)
#define	PDC_TRIGGER_RANDOM_RESET			(0x05 << 24)
#define	PDC_TRIGGER_RANDOM_CENTER			(0x06 << 24)
#define	PDC_TRIGGER_RANDOM_MANUAL			(0x07 << 24)
#define	PDC_TRIGGER_TWOSTAGE				(0x08 << 24)
#define	PDC_TRIGGER_TWOSTAGE_HALF			((0x08 << 24) | 1)
#define	PDC_TRIGGER_TWOSTAGE_QUARTER		((0x08 << 24) | 2)
#define	PDC_TRIGGER_TWOSTAGE_ONEEIGHTH		((0x08 << 24) | 3)
#define	PDC_TRIGGER_RESET					(0x09 << 24)

/*	Sensor Gamma	*/
#define	PDC_SENSOR_GAMMA_1_0				1
#define	PDC_SENSOR_GAMMA_0_9				2
#define	PDC_SENSOR_GAMMA_0_8				3
#define	PDC_SENSOR_GAMMA_0_7				4
#define	PDC_SENSOR_GAMMA_0_6				5
#define	PDC_SENSOR_GAMMA_0_5				6
#define	PDC_SENSOR_GAMMA_0_4				7

/*	Sensor Gain	*/
#define	PDC_SENSOR_GAIN_X1					1
#define	PDC_SENSOR_GAIN_X1_5				2
#define	PDC_SENSOR_GAIN_X2					3
#define	PDC_SENSOR_GAIN_X3					4
#define	PDC_SENSOR_GAIN_X4					5
#define	PDC_SENSOR_GAIN_X6					6
#define	PDC_SENSOR_GAIN_X8					7
#define	PDC_SENSOR_GAIN_X12					8
#define	PDC_SENSOR_GAIN_X16					9
#define	PDC_SENSOR_GAIN_X24					10
#define	PDC_SENSOR_GAIN_X32					11
#define	PDC_SENSOR_GAIN_X64					12

/*	Color Temperature	*/
#define	PDC_COLORTEMP_5100K					1
#define	PDC_COLORTEMP_3100K					2
#define	PDC_COLORTEMP_USER1					3
#define	PDC_COLORTEMP_USER2					4
#define	PDC_COLORTEMP_USER3					5
#define	PDC_COLORTEMP_USER4					6

/*	External Input Signal	*/
#define	PDC_EXT_IN_NONE						0x01
#define	PDC_EXT_IN_CAMSYNC_POSI				0x02
#define	PDC_EXT_IN_CAMSYNC_NEGA				0x03
#define	PDC_EXT_IN_OTHERSSYNC_POSI			0x04
#define	PDC_EXT_IN_OTHERSSYNC_NEGA			0x05
#define	PDC_EXT_IN_EVENT_POSI				0x06
#define	PDC_EXT_IN_EVENT_NEGA				0x07
#define	PDC_EXT_IN_TRIGGER_POSI				0x08
#define	PDC_EXT_IN_TRIGGER_NEGA				0x09
#define	PDC_EXT_IN_READY_POSI				0x0A
#define	PDC_EXT_IN_READY_NEGA				0x0B

/*	External Output Signal	*/
#define	PDC_EXT_OUT_SYNC_POSI				0x01
#define	PDC_EXT_OUT_SYNC_NEGA				0x02
#define	PDC_EXT_OUT_RECORD_POSI				0x03
#define	PDC_EXT_OUT_RECORD_NEGA				0x04
#define	PDC_EXT_OUT_TRIGGER_POSI			0x05
#define	PDC_EXT_OUT_TRIGGER_NEGA			0x06
#define	PDC_EXT_OUT_READY_POSI				0x07
#define	PDC_EXT_OUT_READY_NEGA				0x08
#define	PDC_EXT_OUT_IRIG_RESET_POSI			0x09
#define	PDC_EXT_OUT_IRIG_RESET_NEGA			0x0A
#define	PDC_EXT_OUT_TTLIN_THRU_POSI			0x0B
#define	PDC_EXT_OUT_TTLIN_THRU_NEGA			0x0C
#define	PDC_EXT_OUT_EXPOSE_POSI				0x0D
#define	PDC_EXT_OUT_EXPOSE_NEGA				0x0E
#define	PDC_EXT_OUT_EXPOSE_H1_POSI			0x1D
#define	PDC_EXT_OUT_EXPOSE_H1_NEGA			0x1E
#define	PDC_EXT_OUT_EXPOSE_H2_POSI			0x2D
#define	PDC_EXT_OUT_EXPOSE_H2_NEGA			0x2E
#define	PDC_EXT_OUT_EXPOSE_H3_POSI			0x3D
#define	PDC_EXT_OUT_EXPOSE_H3_NEGA			0x3E
#define	PDC_EXT_OUT_EXPOSE_H4_POSI			0x4D
#define	PDC_EXT_OUT_EXPOSE_H4_NEGA			0x4E

#define	PDC_EXTIO_MAX_PORT					4

/*	Dualslope Shutter	*/
#define	PDC_DSSHUTTER_OFF					0x01
#define	PDC_DSSHUTTER_MODE1					0x02
#define	PDC_DSSHUTTER_MODE2					0x03
#define	PDC_DSSHUTTER_MODE3					0x04
#define	PDC_DSSHUTTER_VALUE_STEP5			0x11

/*	LUT	*/
#define	PDC_LUT_DEFAULT1					1
#define	PDC_LUT_DEFAULT2					2
#define	PDC_LUT_DEFAULT3					3
#define	PDC_LUT_DEFAULT4					4
#define	PDC_LUT_DEFAULT5					5
#define	PDC_LUT_DEFAULT6					6
#define	PDC_LUT_DEFAULT7					7
#define	PDC_LUT_DEFAULT8					8
#define	PDC_LUT_DEFAULT9					9
#define	PDC_LUT_DEFAULT10					10
#define	PDC_LUT_USER1						11
#define	PDC_LUT_USER2						12
#define	PDC_LUT_USER3						13
#define	PDC_LUT_USER4						14

/*	Shading	*/
#define	PDC_SHADING_OFF						1
#define	PDC_SHADING_ON						2
#define	PDC_SHADING_SAVE					3
#define	PDC_SHADING_LOAD					4
#define	PDC_SHADING_UPDATE					5

/*	Pixel Gain	*/
#define	PDC_PIXELGAIN_OFF					0
#define	PDC_PIXELGAIN_NORMAL				1
#define	PDC_PIXELGAIN_SOFT					2
#define	PDC_PIXELGAIN_FLAT					3
#define	PDC_PIXELGAIN_SAVE					4	/*	Reserve	*/
#define	PDC_PIXELGAIN_LOAD					5	/*	Reserve	*/
#define	PDC_PIXELGAIN_SAVE_FILE				6
#define	PDC_PIXELGAIN_LOAD_FILE				7
#define	PDC_PIXELGAIN_NORMAL_64CH			8

/*	Color Plane	*/
#define	PDC_COLOR_PLANE_R					1
#define	PDC_COLOR_PLANE_G					2
#define	PDC_COLOR_PLANE_B					3

/*	I.I. Power	*/
#define	PDC_II_POWER_OFF					1
#define	PDC_II_POWER_ON						2
#define	PDC_II_POWER_ON_LOAD				3

/*	I.I. Gate Select	*/
#define	PDC_II_GATEMODE_OFF					1
#define	PDC_II_GATEMODE_CONTINUOUS			2
#define	PDC_II_GATEMODE_EXTERNAL			3
#define	PDC_II_GATEMODE_GATING				4
#define	PDC_II_GATEMODE_DELAY				5

/*	Edge Enhancement	*/
#define	PDC_EDGE_ENHANCE_OFF				1
#define	PDC_EDGE_ENHANCE_MODE1				2
#define	PDC_EDGE_ENHANCE_MODE2				3
#define	PDC_EDGE_ENHANCE_MODE3				4

/*	Video Output	*/
#define	PDC_VIDEO_OUT_VGA					0x0001
#define	PDC_VIDEO_OUT_NTSC					0x0002
#define	PDC_VIDEO_OUT_PAL					0x0003

/*	Shutter Mode	*/
#define	PDC_SHUTTERLOCK_MODE1				0
#define	PDC_SHUTTERLOCK_MODE2				1

/*	Partition Mode	*/
#define	PDC_PARTITIONINC_MODE1				0
#define	PDC_PARTITIONINC_MODE2				1

/*	8bit select	*/
#define	PDC_8BITSEL_10UPPER					2
#define	PDC_8BITSEL_10MIDDLE				1
#define	PDC_8BITSEL_10LOWER					0
#define	PDC_8BITSEL_12UPPER					4
#define	PDC_8BITSEL_12MIDDLE_U				3
#define	PDC_8BITSEL_12MIDDLE				2
#define	PDC_8BITSEL_12MIDDLE_L				1
#define	PDC_8BITSEL_12LOWER					0
#define	PDC_8BITSEL_8NORMAL					0

/*	Color date tranfer interleave	*/
#define	PDC_COLORDATA_INTERLEAVE_BGR		1
#define	PDC_COLORDATA_INTERLEAVE_RGB		2

/*	Gigabit-Ether I/F Parameter	*/
#define	PDC_GETHER_CONNECT_NORMAL			1
#define	PDC_GETHER_CONNECT_LOWSPEED			0
#define	PDC_GETHER_PACKETSIZE_DEFAULT		0
#define	PDC_GETHER_PACKETSIZE_LOWSPEED		722
#define	PDC_GETHER_TIMEOUT_DEFAULT			5000

/*	Detect mode	*/
#define	PDC_DETECT_NORMAL					0
#define	PDC_DETECT_AUTO						1

/*	Variable pattern num	*/
#define	PDC_VARIABLE_NUM					20

/*	Variable Free Position	*/
#define	PDC_VARIABLE_FREE_CENTER_ONLY		0x00
#define	PDC_VARIABLE_FREE_X					0x01
#define	PDC_VARIABLE_FREE_Y					0x10

/*	Signal Delay	*/
#define	PDC_DELAY_TRIGGER_IN				1
#define	PDC_DELAY_VSYNC_IN					2
#define	PDC_DELAY_GENERAL_IN				3
#define	PDC_DELAY_TRIGGER_OUT_WIDTH			4
#define	PDC_DELAY_VSYNC_OUT					5
#define	PDC_DELAY_VSYNC_OUT_WIDTH			6
#define	PDC_DELAY_EXPOSE					7

/*	Download mode	*/
#define	PDC_DOWNLOAD_MODE_VIDEO_ON			0
#define	PDC_DOWNLOAD_MODE_VIDEO_OFF			1

/*	Camera mode	*/
#define	PDC_CAM_MODE_DEFAULT				0
#define	PDC_CAM_MODE_VARIABLE				1
#define	PDC_CAM_MODE_EXTERNAL				2

/*	Programmable Switch mode	*/
#define	PDC_PROGSWITCH_NONE					0
#define	PDC_PROGSWITCH_RECORDRATE_SEL		1
#define	PDC_PROGSWITCH_RESOLUTION_SEL		2
#define	PDC_PROGSWITCH_SHUTTER_SEL			3
#define	PDC_PROGSWITCH_TRIGGER_MODE_SEL		4
#define	PDC_PROGSWITCH_HEAD_SEL				5
#define	PDC_PROGSWITCH_FIT					6
#define	PDC_PROGSWITCH_STATUS				7
#define	PDC_PROGSWITCH_LIVE					8
#define	PDC_PROGSWITCH_RECREADY				9
#define	PDC_PROGSWITCH_REC					10
#define	PDC_PROGSWITCH_LOWLIGHT				11

#define	PDC_PROGSWITCH_MAX_NUM				4

/*	Color Bayer Convert mode	*/
#define	PDC_BAYERCONVERT_MODE1				0
#define	PDC_BAYERCONVERT_MODE2				1
#define	PDC_BAYERCONVERT_MODE3				2
#define	PDC_BAYERCONVERT_MODE4				3

/*	Sync Out Times	*/
#define	PDC_SYNCOUT_TIMES_X1				1
#define	PDC_SYNCOUT_TIMES_X2				2
#define	PDC_SYNCOUT_TIMES_X4				4
#define	PDC_SYNCOUT_TIMES_X6				6
#define	PDC_SYNCOUT_TIMES_X8				8
#define	PDC_SYNCOUT_TIMES_X10				10

/*	Store Preset	*/
#define	PDC_STORE_PRESET_1					1
#define	PDC_STORE_PRESET_2					2
#define	PDC_STORE_PRESET_3					3
#define	PDC_STORE_PRESET_4					4

/*	Color Enhancement	*/
#define	PDC_COLOR_ENHANCE_OFF				0
#define	PDC_COLOR_ENHANCE_X0_5				1
#define	PDC_COLOR_ENHANCE_X1				2
#define	PDC_COLOR_ENHANCE_X1_5				3
#define	PDC_COLOR_ENHANCE_X2				4

/*	Ethernet	*/
#define	PDC_ETHER_INFO_IP_ADDRESS			0
#define	PDC_ETHER_INFO_NETMASK				1
#define	PDC_ETHER_INFO_GATEWAY				2
#define	PDC_ETHER_INFO_DHCP					3

/*	Head Type	*/
#define	PDC_HEADTYPE_MONO					0
#define	PDC_HEADTYPE_COLOR					1
#define	PDC_HEADTYPE_II						2
#define	PDC_HEADTYPE_UNKNOWN				0xFF

//-->20080204: Nha add
/*	Data Interleave	*/
#define	PDC_COLORDATA_NOCOLOR				0
#define	PDC_COLORDATA_INTERLEAVE_BGR		1
#define	PDC_COLORDATA_INTERLEAVE_RGB		2
//-->
//<--20080315
#define	PDC_CAM_STATUS_DEFAULT				0
#define	PDC_CAM_STATUS_POW_CAB_NOT_CONNECT	1
#define	PDC_CAM_STATUS_CAM_NOT_CONNECT		2
#define	PDC_CAM_STATUS_CL_CONNECT			3
#define	PDC_CAM_STATUS_PoCL_CONNECT			4
//-->

//<!-- 20080419: Cuong add
/*	Live resolution		*/
#define	PDC_LIVE_RESO_FULL					0
#define	PDC_LIVE_RESO_HALF					1
#define PDC_LIVE_RESO_3						2
#define	PDC_LIVE_RESO_QUARTER				3
#define PDC_LIVE_RESO_5						4
#define PDC_LIVE_RESO_6						5
#define PDC_LIVE_RESO_7						6
#define	PDC_LIVE_RESO_DQUARTER				7
//--!>

#endif	/*	_PDC_VALUE_H_	*/

