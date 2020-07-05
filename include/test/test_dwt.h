/* ----------------------------------------------------------------------
 * Project:      DSP Library
 * Title:        test_dwt.h
 * Description:  Public header file for DWT testing
 *
 * $Date:        05. July 2020
 * $Revision:    V.0.0.1
 *
 * Target Processor: Cortex-M7/Cortex-M4/Cortex-M3/Cortex-M0
 * 
 * Python pyWavelets has been used to generate the output to be compared
 * with the arm_dwt_f32 output.
 * @see https://pywavelets.readthedocs.io/en/latest/index.html
 * -------------------------------------------------------------------- */
#ifndef TEST_DWT_H
#define TEST_DWT_H

#include "arm_dwt_f32.h"

/**
  *	@brief DWT test status.
 */
typedef enum
{
	TEST_DWT_SUCCESS,					/**< Generic result code for a success. */
	TEST_DWT_ERROR,						/**< Generic result code for a fail. */
} test_dwt_status;

/**
 * @brief  This function compares two DWT output structures.
 * @param	pExpectedOutput		: pointer to expected output DWT structure (ex : from Python)
 * @param	pTestOutput			: pointer to output DWT structure (ex : from MCU)
 *
 * @note   None.
 * @retval Status of test
 */
test_dwt_status dwt_check_output_f32(
	arm_dwt_out_f32* pExpectedOutput,
	arm_dwt_out_f32* pTestOutput,
	float64_t epsilon );

/**
 * @brief  This function compares two floats relatively to the expected one giving an error.
 * @param	val				: value to compare (ex : from MCU)
 * @param	valExpected		: expected value (ex : from python)
 * @param	epsilon			: tolerance
 *
 * @note   None.
 * @retval Status of test
 */
test_dwt_status relative_error_f32(
	float32_t val,
	float32_t valExpected,
	float64_t epsilon );

/**
 * @brief  This function tests the DWT.
 * @param	epsilon			: tolerance
 *
 * @note   None.
 * @retval None
 */
test_dwt_status dwt_test_f32( 
	float64_t epsilon );

#endif /* TEST_DWT_H */
