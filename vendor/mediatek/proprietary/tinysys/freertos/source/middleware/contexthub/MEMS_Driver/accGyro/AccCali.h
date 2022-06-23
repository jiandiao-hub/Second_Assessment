/*****************************************************************************
* Copyright (C), 2017, MEMSIC Inc.
* File Name		: AccCali.h
* Description	: This file is the head file of AccCali.a. It provides the 
* 			      interface function declarations of the lib file. 
* History		: 1. Data			: 2016/12/22
* 			  	  2. Author			: 
*			  	  3. Modification	: 20180102	
*****************************************************************************/
#ifndef _ACC_CALI_H_
#define _ACC_CALI_H_

#define ANDROID_PLATFORM	1
#define EVB_PLATFORM		2
#define WINDOWS_PLATFORM	3
#define PLATFORM			EVB_PLATFORM

#if((PLATFORM==EVB_PLATFORM)||(PLATFORM==WINDOWS_PLATFORM) ) 
	#define ALOGE(x...)	
#endif

#if(PLATFORM == WINDOWS_PLATFORM)
#define DLL_API extern "C" __declspec(dllexport)
#define STDCALL	_stdcall
#else 
#define DLL_API 
#define STDCALL
#endif

#ifndef MEMSIC_ACC_ALGORITHM_DATA_TYPES
#define MEMSIC_ACC_ALGORITHM_DATA_TYPES
typedef   signed char	int8; 	// signed 8-bit number    (-128 to +127)
typedef unsigned char	uint8; 	// unsigned 8-bit number  (+0 to +255)
typedef   signed short  int16; 	// signed 16-bt number    (-32,768 to +32,767)
typedef unsigned short  uint16; // unsigned 16-bit number (+0 to +65,535)
typedef   signed int	int32; 	// signed 32-bit number   (-2,147,483,648 to +2,147,483,647)
typedef unsigned int	uint32; // unsigned 32-bit number (+0 to +4,294,967,295)
typedef   signed long	int64; 	// signed 64-bit number   (-2,147,483,648 to +2,147,483,647)
typedef unsigned long	uint64; // unsigned 64-bit number (+0 to +4,294,967,295)
#endif

/*******************************************************************************************
* Function Name	: SetAccCaliPara
* Description	: Set offset.
* Input			: para --- default offset.
* Output		: None.
* Return		: 1 --- succeed.
*				 -1 --- fail.
********************************************************************************************/
int SetAccCaliPara(float para);

/*******************************************************************************************
* Function Name	: AccCaliInitial
* Description	: Initial the calibration parameters.
* Input			: defaul direction.
* Output		: None.
* Return		: 1 --- succeed.
*				 -1 --- fail.
********************************************************************************************/
int AccCaliInitial(int dir);

/*******************************************************************************************
* Function Name	: CleanBuffer
* Description	: Clean the static variable in the lib file.
* Input			: None.
* Output		: None.
* Return		: 1 --- succeed.
*				 -1 --- fail.
********************************************************************************************/
int CleanBuffer(void);

/*******************************************************************************************
* Function Name	: DynamicCali
* Description	: Pass the acc raw data, and get the calibrated z axis data.
* Input			: raw[0-1] --- Acceleration X and Y raw data of three axis;
*				  raw[2] --- Acceleration Z axis raw data, make sure z value is the maximum 
*				  			when the phone faces up;
* Output		: output[0] --- Offset of the Z axis.
				  output[1] --- Valid offset to do the AOZ compensation.
				  output[2] --- Calibrated Z axis data .
* Return		: 0 --- calibrating.
*				  1 --- calibrated done.
*				  2 --- need to do the AOZ compensation.
*				 -1 --- fail.
********************************************************************************************/
int DynamicCali(float *raw,float *output);

#endif // _ACC_CALI_H_

