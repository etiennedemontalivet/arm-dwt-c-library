/* ----------------------------------------------------------------------
 * Project:      DSP Library
 * Title:        arm_dwt_f32.c
 * Description:  DWT Floating point process function
 *
 * $Date:        04. July 2020
 * $Revision:    V.0.0.1
 *
 * Target Processor: Cortex-M7/Cortex-M4/Cortex-M3/Cortex-M0
 * -------------------------------------------------------------------- */

#include <arm_dwt_f32.h>

/* ----------------------------------------------------------------------
 * Internal functions
 * -------------------------------------------------------------------- */
/**
 * @brief  This function checks that the DWT internal buffer size is big enough giving a input signal size.
 * @param	pDWTInstance	: pointer to the dwt instance structure
 * @param	blockSize		: size of input signal
 *
 * @note   None
 * @retval arm_dwt_status
 */
arm_dwt_status arm_dwt_check_f32(
	arm_dwt_instance_f32* pDWTInstance,
	uint32_t blockSize )
{
	arm_dwt_status res = DWT_STATUS_ERROR;

	if ( pDWTInstance == NULL )
	{
		res = DWT_STATUS_INVALID_PARAMETER ;
	}
	else
	{
		uint32_t i ;
		uint32_t dwtOutSize = 0 ;
		uint32_t tmpSize ;

		/* Compute full output size */
		tmpSize = blockSize ;
		for ( i = 0 ; i < pDWTInstance->decLevel ; i++ )
		{
			tmpSize = 1 + ( ( tmpSize - 1 ) / 2 ) + ( pDWTInstance->filtersLength / 2 - 1 ) ; // 1 + ((x - 1) / y) --> x/y u32 division and round up
			dwtOutSize += tmpSize ;
		}
		dwtOutSize += tmpSize ;

		/* Check if internal buffer is large enough */
		if ( dwtOutSize <= pDWTInstance->internalBufferSize )
		{
			res = DWT_STATUS_SUCCESS ;
		}
		else
		{
			res = DWT_STATUS_BUFFER_OVERFLOW ; 
		}
	}

	return res;
}

/**
 * @brief  This function performs one wavelet decomposition.
 * @param	pSignal			: pointer to input signal
 * @param	blockSize		: size of input signal
 * @param	pDWTOut			: pointer to the 1-decomposition dwt output structure
 * @param	pDWTInstance	: pointer to the dwt instance structure
 *
 * @note   None
 * @retval arm_dwt_status
 */
arm_dwt_status arm_dwt_1dec_f32(
	float32_t* pSignal,
	uint32_t blockSize,
	arm_dwt_1dec_out_f32* pDWTOut,
	arm_dwt_instance_f32* pDWTInstance )
{
	arm_dwt_status res = DWT_STATUS_ERROR;

	if ( ( pSignal == NULL ) || ( pDWTOut == NULL ) || ( pDWTInstance == NULL ) )
	{
		res = DWT_STATUS_INVALID_PARAMETER;
	}
	else if ( 	( pDWTInstance->pHighFilter 	== NULL )
			|| 	( pDWTInstance->pLowFilter 		== NULL )
			||	( pDWTInstance->pInternalBuffer == NULL )
			||	( pDWTOut->pACoeffs 			== NULL )
			|| 	( pDWTOut->pDCoeffs 			== NULL )
			)
	{
		res = DWT_STATUS_INVALID_PARAMETER;
	}
	else
	{
		/* Local variables: Optimized for readability, not memory footprint. */ 
		uint32_t i , j ;
		uint32_t  outSize = ( blockSize + pDWTInstance->filtersLength - 1 ) / 2 ;
		uint32_t  extendedSignalSize = blockSize + 2 * pDWTInstance->filtersLength - 2 ;
		float32_t extendedSignal[ extendedSignalSize ] ;
		float32_t flippedLowFilter[ pDWTInstance->filtersLength ] ;
		float32_t flippedHighFilter[ pDWTInstance->filtersLength ] ;
		float32_t zeroArray[ pDWTInstance->filtersLength ] ;
		float32_t multRes[ pDWTInstance->filtersLength ] ;
		float32_t cDCoeffs[ outSize ];
		float32_t cACoeffs[ outSize ];
		float32_t preSignal[ pDWTInstance->filtersLength - 2 ];
		float32_t postSignal[ pDWTInstance->filtersLength ];

		/* Flip filters */
		for ( i = 0 ; i < pDWTInstance->filtersLength ; i++ )
		{
			flippedLowFilter [ i ] = pDWTInstance->pLowFilter [ pDWTInstance->filtersLength - i - 1 ] ;
			flippedHighFilter[ i ] = pDWTInstance->pHighFilter[ pDWTInstance->filtersLength - i - 1 ] ;
		}

		/* Create extended signal */
		switch ( pDWTInstance->extrapolationFlag )
		{
		case ARM_DWT_EM_ZERO_PADDING: /**< Zero padding : 		... 0  0    | x1 x2 ... xn | 0  0 ... */
			/* Init arrays that need to be initialized */
			for ( i = 0 ; i < pDWTInstance->filtersLength ; i++ )
			{
				zeroArray[ i ] = 0.0 ;
			}
			/* Fill first filterLength - 1 value with zeros */
			arm_copy_f32( zeroArray, extendedSignal, pDWTInstance->filtersLength - 2 ) ;
			/* Fill with x signal */
			arm_copy_f32( pSignal , &( extendedSignal[ pDWTInstance->filtersLength - 2 ] ) , blockSize ) ;
			/* Fill last filterLength + 1 value with zeros */
			arm_copy_f32( zeroArray, &( extendedSignal[ pDWTInstance->filtersLength - 2 + blockSize ] ), pDWTInstance->filtersLength ) ;

			break;

		case ARM_DWT_EM_SYMMETRIC: /**< Symmetric padding :  	... x2 x1   | x1 x2 ... xn | xn xn-1 ... */
			/* Create and fill padding signals */
			for ( i = 0 ; i < pDWTInstance->filtersLength - 2 ; i++ )
			{
				preSignal[ i ] = pSignal[ pDWTInstance->filtersLength - 3 - i ] ;
			}
			for ( i = 0 ; i < pDWTInstance->filtersLength ; i++ )
			{
				postSignal[ i ] = pSignal[ blockSize - 1 - i ] ;
			}

			/* Fill first filterLength - 1 value with symmetric values */
			arm_copy_f32( preSignal, extendedSignal, pDWTInstance->filtersLength - 2 ) ;
			/* Fill with x signal */
			arm_copy_f32( pSignal , &( extendedSignal[ pDWTInstance->filtersLength - 2 ] ) , blockSize ) ;
			/* Fill last filterLength + 1 value with zeros */
			arm_copy_f32( postSignal, &( extendedSignal[ pDWTInstance->filtersLength - 2 + blockSize ] ), pDWTInstance->filtersLength ) ;
			break;

		default:
			break;
		}

		/* Compute the output cA and cD coefficients */
		for ( i = 0 ; i < outSize ; i++ )
		{
			/* Compute ith cA coefficient */
			arm_mult_f32( &( extendedSignal[ 2 * i ] ), flippedLowFilter, multRes, pDWTInstance->filtersLength ) ;
			cACoeffs[ i ] = 0 ;
			for ( j = 0 ; j < pDWTInstance->filtersLength ; j++ )
			{
				cACoeffs[ i ] += multRes[ j ] ;
			}

			/* Compute ith cD coefficient */
			arm_mult_f32( &( extendedSignal[ 2 * i ] ), flippedHighFilter, multRes, pDWTInstance->filtersLength ) ;
			cDCoeffs[ i ] = 0.0 ;
			for ( j = 0 ; j < pDWTInstance->filtersLength ; j++ )
			{
				cDCoeffs[ i ] += multRes[ j ] ;
			}
		}

		/* Copy result in output */
		arm_copy_f32( cACoeffs, pDWTOut->pACoeffs, outSize );
		arm_copy_f32( cDCoeffs, pDWTOut->pDCoeffs, outSize );
		pDWTOut->size = outSize ;

		res = DWT_STATUS_SUCCESS ;
	}

	return res;
}

/* ----------------------------------------------------------------------
 * Public functions
 * -------------------------------------------------------------------- */
arm_dwt_status arm_dwt_init_f32(
		uint32_t 					filtersLength,
		float32_t* 					pHighFilter,
		float32_t* 					pLowFilter,
		arm_dwt_extrapolation_flag 	extrapolationFlag,
		uint32_t 					decLevel,
		float32_t*					pInternalBuffer,
		uint32_t 					internalBufferSize,
		arm_dwt_instance_f32* pDWTInstance )
{
	arm_dwt_status res = DWT_STATUS_ERROR ;
	if ( 		( pHighFilter 		== NULL )
			||  ( pLowFilter 		== NULL )
			||  ( pInternalBuffer 	== NULL )
			||  ( pDWTInstance 		== NULL ) )
	{
		res = DWT_STATUS_INVALID_PARAMETER ;
	}
	else
	{
		pDWTInstance->filtersLength 		= filtersLength ;
		pDWTInstance->pHighFilter 			= pHighFilter ;
		pDWTInstance->pLowFilter 			= pLowFilter ;
		pDWTInstance->extrapolationFlag 	= extrapolationFlag ;
		pDWTInstance->decLevel				= decLevel ;
		pDWTInstance->pInternalBuffer 		= pInternalBuffer ;
		pDWTInstance->internalBufferSize 	= internalBufferSize ;

		res = DWT_STATUS_SUCCESS;
	}
	return res;
}

arm_dwt_status arm_dwt_f32( 
	arm_dwt_instance_f32* pDWTInstance,
	float32_t* pSignal,
	uint32_t blockSize,
	arm_dwt_out_f32* pDWTOut )
{
	arm_dwt_status res = DWT_STATUS_ERROR;

	if ( ( pSignal == NULL ) || ( pDWTInstance == NULL ) || ( pDWTOut == NULL ) )
	{
		res = DWT_STATUS_INVALID_PARAMETER;
	}
	else if (	 ( pDWTInstance->pHighFilter 		== NULL )
			|| 	 ( pDWTInstance->pLowFilter	 		== NULL )
			||   ( pDWTInstance->pInternalBuffer 	== NULL )
			|| 	 ( arm_dwt_check_f32( pDWTInstance, blockSize ) != DWT_STATUS_SUCCESS ) )
	{
		res = DWT_STATUS_INVALID_PARAMETER;
	}
	else
	{
		uint32_t i ;
		uint32_t copyCounter = 0 ;
		uint32_t decOutputSize = 1 + ( ( blockSize - 1 ) / 2 ) + ( pDWTInstance->filtersLength / 2 - 1 ) ;
		float32_t outCA[ decOutputSize ];
		float32_t outCD[ decOutputSize ];
		arm_dwt_1dec_out_f32 xDecResult = {
				.pACoeffs = outCA,
				.pDCoeffs = outCD
		};

		/* Compute Wavelet decomposition */
		if ( arm_dwt_1dec_f32( pSignal, blockSize, &xDecResult, pDWTInstance ) == DWT_STATUS_SUCCESS )
		{
			/* Copy result into output buffer */
			arm_copy_f32( xDecResult.pDCoeffs, pDWTInstance->pInternalBuffer, xDecResult.size );

			/* Save pointer and size into output result */
			pDWTOut->coeffs[ 0 ].pCoeffs = &( pDWTInstance->pInternalBuffer[ copyCounter ] ) ;
			pDWTOut->coeffs[ 0 ].size = xDecResult.size ;

			/* Update copy index */
			copyCounter += xDecResult.size ;

			/* Recursive decomposition */
			for ( i = 1 ; i < pDWTInstance->decLevel ; i++ )
			{
				/* Compute Wavelet decomposition */
				if ( arm_dwt_1dec_f32( xDecResult.pACoeffs , xDecResult.size , &xDecResult, pDWTInstance ) != DWT_STATUS_SUCCESS )
				{
					break;
				}

				/* Copy result into output buffer */
				arm_copy_f32( xDecResult.pDCoeffs, &( pDWTInstance->pInternalBuffer[ copyCounter ] ) , xDecResult.size );

				/* Save pointer and size into output result */
				pDWTOut->coeffs[ i ].pCoeffs = &( pDWTInstance->pInternalBuffer[ copyCounter ] ) ;
				pDWTOut->coeffs[ i ].size = xDecResult.size ;

				/* Update copy index */
				copyCounter += xDecResult.size ;
			}

			/* Test if everything were right */
			if ( i == pDWTInstance->decLevel )
			{
				/* Copy last approximation coefficents */
				arm_copy_f32( xDecResult.pACoeffs, &( pDWTInstance->pInternalBuffer[ copyCounter ] ) , xDecResult.size );

				/* Save pointer and size into output result */
				pDWTOut->coeffs[ pDWTInstance->decLevel ].pCoeffs = &( pDWTInstance->pInternalBuffer[ copyCounter ] ) ;
				pDWTOut->coeffs[ pDWTInstance->decLevel ].size = xDecResult.size ;

				/* Update copy index */
				copyCounter += xDecResult.size ;

				/* Save number of level */
				pDWTOut->decLevel = pDWTInstance->decLevel ;

				res = DWT_STATUS_SUCCESS ;
			}
		}
	}

	return res;
}
