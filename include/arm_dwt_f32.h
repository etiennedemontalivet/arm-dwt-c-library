/* ----------------------------------------------------------------------
 * Project:      DSP Library
 * Title:        arm_dwt_f32.h
 * Description:  Public header file for DWT processing
 *
 * $Date:        04. July 2020
 * $Revision:    V.0.0.1
 *
 * Target Processor: Cortex-M7/Cortex-M4/Cortex-M3/Cortex-M0
 * -------------------------------------------------------------------- */

#ifndef ARM_DWT_H
#define ARM_DWT_H

/* Includes ------------------------------------------------------------------------------------------------------------------------------- */
#include "arm_math.h"

#define ARM_DWT_MAX_DEC_LEVEL		10		/**< Maximum level of DWT decomposition (to prevent memory leak. Can be increased). */

/**
  *	@brief DWT computation status result.
 */
typedef enum
{
	DWT_STATUS_SUCCESS,					/**< Generic result code for a success. */
	DWT_STATUS_ERROR,					/**< Generic result code for a error. */
	DWT_STATUS_INVALID_PARAMETER,		/**< Generic result code for an invalid parameter. */
	DWT_STATUS_BUFFER_OVERFLOW,			/**< Result code for DWT pre-allocated output buffer overflow. */
} arm_dwt_status;

/**
  *	@brief DWT signal extrapolation method for signal padding.
 */
typedef enum
{
	ARM_DWT_EM_ZERO_PADDING,				/**< Zero padding extrapolation : 		... 0  0    | x1 x2 ... xn | 0  0 ... */
	ARM_DWT_EM_SYMMETRIC,					/**< Symmetric padding :  				... x2 x1   | x1 x2 ... xn | xn xn-1 ... */
//	ARM_DWT_EM_CONSTANT_PADDING,			/**< Constant padding extrapolation : 	... x1 x1   | x1 x2 ... xn | xn xn ... */
//	ARM_DWT_EM_PERIODIC,					/**< Periodic padding : 				... xn-1 xn | x1 x2 ... xn | x1 x2 ... */
} arm_dwt_extrapolation_flag;

/**
  *	@brief DWT instance for floating point.
 */
typedef struct
{
	uint32_t 					filtersLength;				/**< Length of high and low filters. */
	float32_t* 					pHighFilter;				/**< Pointer to high pass filter. */
	float32_t* 					pLowFilter;					/**< Pointer to low pass filter. */
	arm_dwt_extrapolation_flag 	extrapolationFlag;			/**< Extrapolation method. */
	uint32_t 					decLevel;					/**< Number of DWT level. */
	float32_t*					pInternalBuffer;			/**< Pointer to internal buffer (where the coefficients will be stored). */
	uint32_t 					internalBufferSize;			/**< Size of internal buffer. */
} arm_dwt_instance_f32;

/**
  *	@brief 1-decomposition DWT output structure.
 */
typedef struct
{
	uint32_t 					size;					/**< Length of DWT coefficients arrays. */
	float32_t* 					pDCoeffs;				/**< Pointer to DWT details coefficients. */
	float32_t* 					pACoeffs;				/**< Pointer to DWT approx coefficients. */
} arm_dwt_1dec_out_f32;

/**
  *	@brief DWT coefficient array structure. Used for cD1, cD2, ...cDn and cA.
 */
typedef struct
{
	float32_t* 					pCoeffs;				/**< Pointer to DWT coefficients. */
	uint32_t 					size;					/**< Number of coefficients. */
} arm_dwt_coeffs_f32;

/**
  *	@brief Full DWT output structure.
 */
typedef struct
{
	arm_dwt_coeffs_f32			coeffs[ ARM_DWT_MAX_DEC_LEVEL + 1 ];	/**< DWT coefficients. Range in order : [ [ cD1 ] , [ cD2 ] , ... [ cDn ] , [ cAn] ], n = level */
	uint32_t 					decLevel;								/**< Level of DWT  */
} arm_dwt_out_f32;


/* Public function prototypes--------------------------------------------------------------------------------------------------------------------- */
/**
 * @brief  This function initializes the dwt.
 * @param	filtersLength			: Length of high and low filters
 * @param	pHighFilter				: Pointer to high pass filter.
 * @param	pLowFilter				: Pointer to low pass filter.
 * @param	extrapolationFlag		: Extrapolation method
 * @param	decLevel				: Number of DWT level
 * @param	pInternalBuffer			: Pointer to internal buffer
 * @param	internalBufferSize		: Size of internal buffer
 * @param	pDWTInstance			: pointer to the dwt instance structure
 *
 * @note   None.
 * @retval arm_dwt_status
 */
arm_dwt_status arm_dwt_init_f32(
		uint32_t 					filtersLength,
		float32_t* 					pHighFilter,
		float32_t* 					pLowFilter,
		arm_dwt_extrapolation_flag 	extrapolationFlag,
		uint32_t 					decLevel,
		float32_t*					pInternalBuffer,
		uint32_t 					internalBufferSize,
		arm_dwt_instance_f32* 		pDWTInstance );

/**
 * @brief  This function runs the dwt computation.
 * @param	pDWTInstance	: pointer to the dwt instance structure
 * @param	pSignal			: pointer to input signal
 * @param	blockSize		: size of input signal
 * @param	pOut			: pointer to the full dwt output structure
 *
 * @note   None
 * @retval arm_dwt_status
 */
arm_dwt_status arm_dwt_f32(
		arm_dwt_instance_f32* pDWTInstance,
		float32_t* pSignal,
		uint32_t blockSize,
		arm_dwt_out_f32* pOut ) ;

#endif /* ARM_DWT_H */
