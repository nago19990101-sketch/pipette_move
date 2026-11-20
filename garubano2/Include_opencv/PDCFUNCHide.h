/*
 *	PDCFUNCHide.h
 *	Photron Device Control SDK
 *	Function Definition (Internal Function)
 *
 *	Copyright (C) 2006-2007 PHOTRON LIMITED
 */

#ifndef	_PDC_FUNC_HIDE_H_
#define	_PDC_FUNC_HIDE_H_

#ifdef	__cplusplus
extern "C" {
#endif	/*	__cplusplus	*/

/*
 *	Get Structure
 */
unsigned long WINAPI
PDC_GetDeviceInfo(
		unsigned long			nDeviceNo,
		PPDC_DEVICE_INFO		pDeviceInfo,
		unsigned long			*pErrorCode
		);

unsigned long WINAPI
PDC_GetChildInfo(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		PPDC_CHILDDEVICE_INFO	pChildInfo,
		unsigned long			*pErrorCode
		);
unsigned long WINAPI
PDC_GetChildMemoryInfo(
		unsigned long			nDeviceNo,
		unsigned long			nChildNo,
		PPDC_CHILDDEVICE_MEMORY_INFO	pChildInfo,
		unsigned long			*pErrorCode
		);


/*
 *	Get Device status
 */
unsigned long WINAPI
PDC_ReqDeviceID(
		unsigned long	nDeviceNo,
		unsigned long	*pID,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqProductID(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pID,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqLotID(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pID,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqIndividualID(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pID,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqVersion(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pVer,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqColor(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		char			*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqShutterLockMode(
		unsigned long	nDeviceNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqPartitionIncMode(
		unsigned long	nDeviceNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqKeyPadLock(
		unsigned long	nDeviceNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqVideoOutMode(
		unsigned long	nDeviceNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqChildDeviceCount(
		unsigned long	nDeviceNo,
		unsigned long	*pCount,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMaxChildDeviceCount(
		unsigned long	nDeviceNo,
		unsigned long	*pCount,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqExternalCount(
		unsigned long	nDeviceNo,
		unsigned long	*pIn,
		unsigned long	*pOut,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqExternalIn(
		unsigned long	nDeviceNo,
		unsigned long	nPort,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqExternalOut(
		unsigned long	nDeviceNo,
		unsigned long	nPort,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqIRIG(
		unsigned long	nDeviceNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMCDL(
		unsigned long	nDeviceNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqIRIGOffset(
		unsigned long	nDeviceNo,
		long			*pValue,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqRecordRate(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pRate,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqTriggerMode(
		unsigned long	nDeviceNo,
		unsigned long	*pMode,
		unsigned long	*pAFrames,
		unsigned long	*pRFrames,
		unsigned long	*pRCount,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqResolution(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pWidth,
		unsigned long	*pHeight,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqShutterSpeedFps(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pFps,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqShutterSpeedUsec(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pUsec,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMaxPartition(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pCount,
		unsigned long	*pBlock,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqCurrentPartition(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pNo,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqSensorGainMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqSensorGammaMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqColorTempMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pR,
		unsigned long	*pG,
		unsigned long	*pB,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqAutoExposure(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqLUTMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqDSShutterMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqDSShutterValue(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pValue,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqEdgeEnhanceMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqShadingMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqPixelGainMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMaxFrames(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pFrames,
		unsigned long	*pBlocks,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMaxResolution(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pWidth,
		unsigned long	*pHeight,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqSegmentPosition(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pXPos,
		unsigned long	*pYPos,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMaxBitDepth(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		char			*pDepth,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqDelay(
		unsigned long	nDeviceNo,
		unsigned long	nSignal,
		unsigned long	*pValue,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqDelayLimit(
		unsigned long	nDeviceNo,
		unsigned long	nSignal,
		unsigned long	*pValue,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqLowLightMode(
		unsigned long	nDeviceNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqProgSwitchMode(
		unsigned long	nDeviceNo,
		unsigned long	nSwitchNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqColorTempBase(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pBase,
		unsigned long	*pMax,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqColorEnhanceMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqDownloadMode(
		unsigned long	nDeviceNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqCamMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqVariableFunction(
		unsigned long	nDeviceNo,
		char			*pFlag,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMaxFullResoRate(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pRate,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqSyncOutTimes(
		unsigned long	nDeviceNo,
		unsigned long	*pRatio,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqStoreNum(
		unsigned long	nDeviceNo,
		unsigned long	*pNum,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqIRIGPhaseLock(
		unsigned long	nDeviceNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqIRIGPhaseLockStatus(
		unsigned long	nDeviceNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

/*
 *	Get Recorded status
 */
unsigned long WINAPI
PDC_ReqMemIRIG(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemMCDL(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemRecordRate(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pRate,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemTriggerMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pAFrames,
		unsigned long	*pRFrames,
		unsigned long	*pRCount,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemResolution(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pWidth,
		unsigned long	*pHeight,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemShutterSpeedFps(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pFps,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemSensorGainMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemSensorGammaMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemColorTempMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pR,
		unsigned long	*pG,
		unsigned long	*pB,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemAutoExposure(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemLUTMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemDSShutterMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemDSShutterValue(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pValue,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemEdgeEnhanceMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqFrameZero(
		unsigned long	nDeviceNo,
		char			*pFlag,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemEventFrame(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pCount,
		long			*pFrames,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemEventRCount(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pCount,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemTwoStageTriggerFrame(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pLowToHigh,
		unsigned long	*pHighToLow,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemTwoStageTriggerTiming(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pTiming,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemRecordedFrames(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pFrames,
		unsigned long	*pErrorCode
		);
//<--20081120: Nha add
unsigned long WINAPI 
PDC_ReqMemRecordedFramesPerTrigger(
	   unsigned long nDeviceNo,
	   unsigned long nChildNo,
	   unsigned long nTriggerNo,
	   unsigned long *pFrameNumPerTrg,
	   unsigned long *pErrorCode
	   );
//-->
unsigned long WINAPI
PDC_ReqMemFrameInfo(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		PPDC_FRAME_INFO	pFrame,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqColorEnhanceValue(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	nPlane,
		long			*pR,
		long			*pG,
		long			*pB,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemColorEnhanceValue(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	nPlane,
		long			*pR,
		long			*pG,
		long			*pB,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemSegmentPosition(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pXPos,
		unsigned long	*pYPos,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemColorEnhanceMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemIRIGPhaseLockStatus(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqHighSpeedMode(
		unsigned long	nDeviceNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

/*
 *	Multi Head Cache Status
 */
unsigned long WINAPI
PDC_ReqRecordRateCache(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pRate,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqResolutionCache(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pWidth,
		unsigned long	*pHeight,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMaxFramesCache(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pFrames,
		unsigned long	*pBlocks,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqMemRecordedFramesCache(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pFrames,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqSegmentPositionCache(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pXPos,
		unsigned long	*pYPos,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_ReqCamModeCache(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

/*
 *	Pixel Def Revision
 */
unsigned long WINAPI
PDC_GetPixelDefRevisionList(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pSize,
		unsigned long	*pList,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_SetPixelDefRevisionList(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	nSize,
		unsigned long	*pList,
		unsigned long	*pSize,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_GetPixelDefRevision(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_SetPixelDefRevision(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	nMode,
		unsigned long	*pErrorCode
		);

/*
 *	ETC
 */
unsigned long WINAPI
PDC_GetPixelValue(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	nXPos,
		unsigned long	nYPos,
		unsigned long	*pRValue,
		unsigned long	*pGValue,
		unsigned long	*pBValue,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_GetTransferPixelBuf(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_SetTransferPixelBuf(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	nMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_GetBayerConvertMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_SetBayerConvertMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	nMode,
		unsigned long	*pErrorCode
		);

unsigned long WINAPI
PDC_GetLiveImageCacheData(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		void			*pData,
		unsigned long	*pErrorCode
		);

//<!-- 20080419: Cuong Add
unsigned long WINAPI
PDC_ReqLiveResolutionMode(
		unsigned long	nDeviceNo,
		unsigned long	nChildNo,
		unsigned long	*pMode,
		unsigned long	*pErrorCode
		);
//--!>


#ifdef	__cplusplus
}
#endif	/*	__cplusplus	*/

#endif	/*	_PDC_FUNC_HIDE_H_	*/

