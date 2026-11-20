/*
 *	PDCFUNC.h
 *	Photron Device Control SDK
 *	Function Definition
 *
 *	Copyright (C) 2006-2007 PHOTRON LIMITED
 */

#ifndef	_PDC_FUNC_H_
#define	_PDC_FUNC_H_

#ifdef	__cplusplus
extern "C" {
#endif	/*	__cplusplus	*/

/*
 *	Initialize etc...
 */
unsigned long WINAPI
PDC_Init(
		unsigned long 			*pErrorCode
		);

unsigned long WINAPI
PDC_DetectDevice(
		unsigned long			nInterfaceCode,
		unsigned long			*pDetectNo,
		unsigned long			nDetectNum,
		unsigned long			nDetectParam,
		PPDC_DETECT_NUM_INFO	pDetectNumInfo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_OpenDevice(
		PPDC_DETECT_INFO		pDetectInfo,
		unsigned long			*pDeviceNo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_CloseDevice(
		unsigned long			nDeviceNo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetDeviceName(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		char					*pStrName,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetNumberOfDevice(
		unsigned long			*pNum,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_IsFunction(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nFunction,
		char					*pFlag,
		unsigned long			*pErrorCode
		);


unsigned long WINAPI
PDC_SetInterfaceInfo(
		unsigned long			nInterfaceCode,
		unsigned long			nParam1,
		unsigned long			nParam2,
		unsigned long			nParam3,
		unsigned long			nParam4,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetInterfaceInfo(
		unsigned long			nInterfaceCode,
		unsigned long			*pParam1,
		unsigned long			*pParam2,
		unsigned long			*pParam3,
		unsigned long			*pParam4,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_DeviceStatusUpdate(
		unsigned long			nDeviceNo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetHeadTypeName(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nHeadNo,
		char					*pStrName,
		unsigned long			*pErrorCode
		);

/*
 *	Get List
 */
unsigned long WINAPI
PDC_GetRecordRateList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetResolutionList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetShutterSpeedFpsList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetShutterSpeedUsecList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetTriggerModeList(
		unsigned long			nDeviceNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetSensorGainModeList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetSensorGammaModeList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetColorTempModeList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetLUTModeList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVariableRecordRateList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetDSShutterModeList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVideoOutModeList(
		unsigned long			nDeviceNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetShadingModeList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetAEShutterFpsList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetAEShutterUsecList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetEdgeEnhanceModeList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetExternalInModeList(
		unsigned long			nDeviceNo,
		unsigned long			nPortNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetExternalOutModeList(
		unsigned long			nDeviceNo,
		unsigned long			nPortNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetExistChildDeviceList(
		unsigned long			nDeviceNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetOpenedDeviceList(
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetLUTValueList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nPlane,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetLUTOrgValueList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nPlane,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetPartitionList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pCount,
		unsigned long			*pFrames,
		unsigned long			*pBlocks,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetProgSwitchModeList(
		unsigned long			nDeviceNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetPixelGainModeList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetSyncOutTimesList(
		unsigned long			nDeviceNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetColorEnhanceModeList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetHeadExchangeList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pSize,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

/*
 *	Set List
 */
unsigned long WINAPI
PDC_SetLUTValueList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nPlane,
		unsigned long			*pList,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetPartitionList(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nCount,
		unsigned long			*pBlocks,
		unsigned long			*pErrorCode
		);


/*
 *	Get Device status
 */
unsigned long WINAPI
PDC_GetDeviceCode(
		unsigned long			nDeviceNo,
		unsigned long			*pCode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetDeviceID(
		unsigned long			nDeviceNo,
		unsigned long			*pID,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetProductID(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pID,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetLotID(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pID,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetIndividualID(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pID,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVersion(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pVer,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetColorType(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		char					*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetStatus(
		unsigned long			nDeviceNo,
		unsigned long			*pStatus,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetShutterLockMode(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetShutterSpeedUsec(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pUsec,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetPartitionIncMode(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetKeyPadLock(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVideoOutMode(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetChildDeviceCount(
		unsigned long			nDeviceNo,
		unsigned long			*pCount,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMaxChildDeviceCount(
		unsigned long			nDeviceNo,
		unsigned long			*pCount,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetExternalCount(
		unsigned long			nDeviceNo,
		unsigned long			*pIn,
		unsigned long			*pOut,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetExternalInMode(
		unsigned long			nDeviceNo,
		unsigned long			nPort,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetExternalOutMode(
		unsigned long			nDeviceNo,
		unsigned long			nPort,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetIRIG(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMCDL(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetIRIGOffset(
		unsigned long			nDeviceNo,
		long					*pValue,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetRecordRate(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pRate,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetTriggerMode(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pAFrames,
		unsigned long			*pRFrames,
		unsigned long			*pRCount,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetResolution(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pWidth,
		unsigned long			*pHeight,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetShutterSpeedFps(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pFps,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMaxPartition(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pCount,
		unsigned long			*pBlock,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetCurrentPartition(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pNo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetSensorGainMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetSensorGammaMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetColorTempMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pR,
		unsigned long			*pG,
		unsigned long			*pB,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetAutoExposure(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetLUTMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetDSShutterMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetDSShutterValue(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pValue,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetEdgeEnhanceMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetShadingMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetPixelGainMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMaxFrames(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pFrames,
		unsigned long			*pBlocks,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMaxResolution(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pWidth,
		unsigned long			*pHeight,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetSegmentPosition(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pXPos,
		unsigned long			*pYPos,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVariableMaxResolution(
		unsigned long			nDeviceNo,
		unsigned long			nRate,
		unsigned long			*pWidth,
		unsigned long			*pHeight,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVariableMaxWidth(
		unsigned long			nDeviceNo,
		unsigned long			nRate,
		unsigned long			nHeight,
		unsigned long			*pWidth,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVariableMaxHeight(
		unsigned long			nDeviceNo,
		unsigned long			nRate,
		unsigned long			nWidth,
		unsigned long			*pHeight,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVariableMaxRate(
		unsigned long			nDeviceNo,
		unsigned long			nWidth,
		unsigned long			nHeight,
		unsigned long			*pRate,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVariableChannel(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pChannel,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVariableChannelInfo(
		unsigned long			nDeviceNo,
		unsigned long			nChannel,
		unsigned long			*pRate,
		unsigned long			*pWidth,
		unsigned long			*pHeight,
		unsigned long			*pXPos,
		unsigned long			*pYPos,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVariableChannelInfo2(
		unsigned long			nDeviceNo,
		unsigned long			nChannel,
		unsigned long			*pRate,
		unsigned long			*pWidth,
		unsigned long			*pHeight,
		unsigned long			*pXPos,
		unsigned long			*pYPos,
		unsigned long			*pIRIG,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVariableChannelInfo3(
		unsigned long			nDeviceNo,
		unsigned long			nChannel,
		unsigned long			*pRate,
		unsigned long			*pWidth,
		unsigned long			*pHeight,
		unsigned long			*pXPos,
		unsigned long			*pYPos,
		unsigned long			*pIRIG,
		unsigned long			*pHSMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetAutoExposureInfo(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pWidth,
		unsigned long			*pHeight,
		unsigned long			*pXPos,
		unsigned long			*pYPos,
		unsigned long			*pValue,
		unsigned long			*pRange,
		unsigned long			*pLimit,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMaxBitDepth(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		char					*pDepth,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetLUTParams(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nLUTMode,
		PPDC_LUT_PARAMS			pParams,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetDelay(
		unsigned long			nDeviceNo,
		unsigned long			nSignal,
		unsigned long			*pValue,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetDelayLimit(
		unsigned long			nDeviceNo,
		unsigned long			nSignal,
		unsigned long			*pValue,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetVariableRestriction(
		unsigned long			nDeviceNo,
		unsigned long			*pWidthStep,
		unsigned long			*pHeightStep,
		unsigned long			*pXPosStep,
		unsigned long			*pYPosStep,
		unsigned long			*pWidthMin,
		unsigned long			*pHeightMin,
		unsigned long			*pFreePos,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetColorEnhanceValue(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nPlane,
		long					*pR,
		long					*pG,
		long					*pB,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetColorEnhanceMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetDownloadMode(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetCamMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMaxFullResoRate(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pRate,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetLowLightMode(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetProgSwitchMode(
		unsigned long			nDeviceNo,
		unsigned long			nSwitchNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetColorTempBase(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pBase,
		unsigned long			*pMax,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetAutoExposureRestriction(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pWidthStep,
		unsigned long			*pHeightStep,
		unsigned long			*pXPosStep,
		unsigned long			*pYPosStep,
		unsigned long			*pWidthMin,
		unsigned long			*pHeightMin,
		unsigned long			*pMaxValue,
		unsigned long			*pMaxRange,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetSyncOutTimes(
		unsigned long			nDeviceNo,
		unsigned long			*pRatio,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetStoreNum(
		unsigned long			nDeviceNo,
		unsigned long			*pNum,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetEthernetInfo(
		unsigned long			nDeviceNo,
		unsigned long			nTarget,
		unsigned long			*pValue,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetIRIGPhaseLock(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetIRIGPhaseLockStatus(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetHighSpeedMode(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetBurstTransfer(
		unsigned long			nDeviceNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetHeadExchange(
		unsigned long			nDeviceNo,
		unsigned long			*pNo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetStepShutter(
		unsigned long			nDeviceNo,
		unsigned long			*pStep,
		unsigned long			*pMin,
		unsigned long			*pMax,
		unsigned long			*pErrorCode
		);

/*
 *	Get Recorded status
 */
unsigned long WINAPI
PDC_GetMemIRIG(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemMCDL(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemRecordRate(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pRate,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemTriggerMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pAFrames,
		unsigned long			*pRFrames,
		unsigned long			*pRCount,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemResolution(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pWidth,
		unsigned long			*pHeight,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemShutterSpeedFps(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pFps,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemSensorGainMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemSensorGammaMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemColorTempMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pR,
		unsigned long			*pG,
		unsigned long			*pB,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemAutoExposure(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemLUTMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemDSShutterMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemDSShutterValue(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pValue,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemEdgeEnhanceMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetTransferOption(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*p8BitSel,
		unsigned long			*pBayer,
		unsigned long			*pInterleave,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemFrameInfo(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		PPDC_FRAME_INFO			pFrame,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemColorEnhanceValue(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nPlane,
		long					*pR,
		long					*pG,
		long					*pB,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemColorEnhanceMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemIRIGPhaseLockStatus(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pMode,
		unsigned long			*pErrorCode
		);

/*
 *	Set Device status
 */
unsigned long WINAPI
PDC_SetActiveChild(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetStatus(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetRecReady(
		unsigned long			nDeviceNo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetEndless(
		unsigned long			nDeviceNo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_TriggerIn(
		unsigned long			nDeviceNo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetShutterLockMode(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetPartitionIncMode(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetKeyPadLock(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetVideoOutMode(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetExternalInMode(
		unsigned long			nDeviceNo,
		unsigned long			nPort,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetExternalOutMode(
		unsigned long			nDeviceNo,
		unsigned long			nPort,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetIRIG(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetMCDL(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetIRIGOffset(
		unsigned long			nDeviceNo,
		long					nValue,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetRecordRate(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nRate,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetTriggerMode(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			nAFrames,
		unsigned long			nRFrames,
		unsigned long			nRCount,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetResolution(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nWidth,
		unsigned long			nHeight,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetShutterSpeedFps(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nFps,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetCurrentPartition(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nNo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetSensorGainMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetSensorGammaMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetColorTempMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		unsigned long			nR,
		unsigned long			nG,
		unsigned long			nB,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetAutoExposure(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetLUTMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetDSShutterMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetDSShutterValue(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nValue,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetEdgeEnhanceMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetShadingMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetPixelGainMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetTransferOption(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			n8BitSel,
		unsigned long			nBayer,
		unsigned long			nInterleave,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_EraseVariableChannel(
		unsigned long			nDeviceNo,
		unsigned long			nChannel,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetVariableChannel(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nChannel,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetVariableChannelInfo(
		unsigned long			nDeviceNo,
		unsigned long			nChannel,
		unsigned long			nRate,
		unsigned long			nWidth,
		unsigned long			nHeight,
		unsigned long			nXPos,
		unsigned long			nYPos,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetAutoExposureInfo(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nWidth,
		unsigned long			nHeight,
		unsigned long			nXPos,
		unsigned long			nYPos,
		unsigned long			nValue,
		unsigned long			nRange,
		unsigned long			nLimit,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetLUTUserParams(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nLUTMode,
		PPDC_LUT_PARAMS			pParams,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_CreatePixelGainData(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		void					*pSrc,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetPixelGainData(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		void					*pSrc,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetLUTUser(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetDelay(
		unsigned long			nDeviceNo,
		unsigned long			nSignal,
		unsigned long			nValue,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetColorEnhanceMode(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetDownloadMode(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetLowLightMode(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetProgSwitchMode(
		unsigned long			nDeviceNo,
		unsigned long			nSwitchNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetSyncOutTimes(
		unsigned long			nDeviceNo,
		unsigned long			nRatio,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetStore(
		unsigned long			nDeviceNo,
		unsigned long			nNo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetEthernetInfo(
		unsigned long			nDeviceNo,
		unsigned long			nTarget,
		unsigned long			nValue,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetIRIGPhaseLock(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetHighSpeedMode(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetBurstTransfer(
		unsigned long			nDeviceNo,
		unsigned long			nMode,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_SetHeadExchange(
		unsigned long			nDeviceNo,
		unsigned long			nNo,
		unsigned long			*pErrorCode
		);

/*
 *	Get Image
 */
unsigned long WINAPI
PDC_GetLiveImageData(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nBitDepth,
		void					*pData,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetLiveImageDataStart(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetLiveImageDataEnd(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nBitDepth,
		void					*pData,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemImageData(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		long					nFrameNo,
		unsigned long			nBitDepth,
		void					*pData,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemImageDataStart(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		long					nFrameNo,
		unsigned long			nBitDepth,
		void					*pData,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemImageDataEnd(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nBitDepth,
		void					*pData,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemIRIGData(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		long					nFrameNo,
		PPDC_IRIG_INFO			pData,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemMCDLData(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		long					nFrameNo,
		PPDC_MCDL_INFO			pData,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetMemIRIGandMCDLData(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		long					nFrameNo,
		unsigned long			nFrameNum,
		PPDC_IRIGMCDL_INFO		pData,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetPixelGainData(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		unsigned long			nMode,
		void					*pData,
		unsigned long			*pErrorCode
		);

//<--20080404: Nha add
unsigned long WINAPI
PDC_FPGAConfig(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		LPCTSTR					lpszPath,
		unsigned long			m_ProgramMode,
		//unsigned long			*nSize,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetStatusFPGAConf(
					unsigned long			nDeviceNo,
					unsigned long			nChildNo,
					unsigned long			*pStatus,
					unsigned long 			*pBytesCompleted,
					unsigned long			*pErrorCode
					 );

unsigned long WINAPI
PDC_SetReset_FPGA(unsigned long			nDeviceNo,
				  unsigned long			nChildNo,
				  unsigned long			nEnable,
				  unsigned long			nFPGAType,
				  unsigned long			*pErrorCode
  			     );
//-->

//<!-- 20080118: Thao add
unsigned long WINAPI
PDC_ReadRegister(
		unsigned long			nDeviceNo,
		unsigned long			nAddr,
		unsigned long			AccessType
		);

unsigned long WINAPI
PDC_WriteRegister(
		unsigned long			nDeviceNo,
		unsigned long			nAddr,
		unsigned long			nValue,
		unsigned long			AccessType
		);
//--!>

//<--20080127: Nha add
unsigned long WINAPI
PDC_BMPFileSave(
		LPCTSTR					lpszFileName,
		PPDC_IMAGE_BUFF_INFO	pInfo,
		unsigned char			*pBuff,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_AVIFileSaveOpen(
		LPCTSTR					lpszFileName,
		PPDC_IMAGE_BUFF_INFO	pInfo,
		long					nRate,
		LPAVICOMPRESSOPTIONS	pOptions,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_AVIFileSave(
		unsigned long			nNumber,
		unsigned char			*pBuff,
		unsigned long			*pSize,
		unsigned long			*pErrorCode
		);

void WINAPI
PDC_AVIFileSaveClose();
//-->
//<--20080315:Hien
unsigned long WINAPI 
PDC_GetCamStatus(unsigned long nDeviceNo,
									unsigned long nChildNo,
									unsigned long *pMode,
									unsigned long *pErrorCode);
//-->

//<--20080320: Cuong
unsigned long WINAPI PDC_SetCommand(unsigned long nDeviceNo,
									unsigned long nChildNo,									
									unsigned char *pBuf,
									unsigned long nSize,
									unsigned long *pErrorCode);
//-->

//<!--20080419: Cuong
unsigned long WINAPI 
PDC_GetLiveResolutionList(
		unsigned long nDeviceNo,
		unsigned long nChildNo,
		unsigned long *pSize,
		unsigned long *pList,
		unsigned long *pErrorCode
		);

unsigned long WINAPI 
PDC_SetLiveResolutionMode( 
		unsigned long nDeviceNo,
		unsigned long nChildNo,
		unsigned long nMode,
		unsigned long *pErrorCode
		);

unsigned long WINAPI 
PDC_GetLiveResolutionMode(
		unsigned long nDeviceNo,
		unsigned long nChildNo,
		unsigned long *pMode,
		unsigned long *pErrorCode);

unsigned long WINAPI 
PDC_GetMemLiveResolutionMode(
		unsigned long nDeviceNo,
		unsigned long nChildNo,
		unsigned long *pMode,
		unsigned long *pErrorCode);
//--!>

//<!--20080513: Cuong
unsigned long WINAPI 
PDC_GetMemory(
		unsigned long nDeviceNo,
		unsigned long nChildNo,									
		unsigned long *pFrameAvailable,
		unsigned long *pErrorCode);

unsigned long WINAPI 
PDC_SetMemory(
		unsigned long nDeviceNo,
		unsigned long nChildNo,
		unsigned long nOption,
		unsigned long nBlocks,
		unsigned long nFramePerBlock,
		unsigned long *pErrorCode);
//--!>

//<!--20080605: Cuong
unsigned long WINAPI
PDC_GetMemFrameAddress(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		long					nFrameNo,		
		void					**pBaseAddress,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI 
PDC_GetLiveFrameAddress(
		unsigned long nDeviceNo,
		unsigned long nChildNo,
		unsigned long *pFrameNo,
		void		  **pBaseAddress,
		unsigned long *pErrorCode
		);
//--!>

//<!--20080710
unsigned long WINAPI
PDC_GetFrameAddress(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		long					nFrameNo,		
		void					**pBaseAddress,
		unsigned long			*pErrorCode
		);
//--!>

#ifdef	__cplusplus
}
#endif	/*	__cplusplus	*/

#endif	/*	_PDC_FUNC_H_	*/

