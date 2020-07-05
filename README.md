# arm-dwt-c-library
A C implementation of the one dimensional *Discrete Wavelet Transform* (DWT) based on CMSIS library.

## How to use
Add `arm_dwt_f32.h/c` to your project. Link the [CMSIS library](https://github.com/ARM-software/CMSIS_5)

Then just call the init function `arm_dwt_init_f32( )` with the right parameters, then call `arm_dwt_f32( )`. To see an example, please refer to the test I made.

*Note*: I did not provide a project or a Makefile because you can easily only add the files to your project. Moreover, the project would be target specific (for instance, to use the CMSIS library, you have to specify the cortex you are running your code on).

## Implementation
The implementation is inspired by [PyWavelets](https://pywavelets.readthedocs.io/en/latest/). The code has not been optimized for speed but for readability.
Depending on your decomposition level, don't forget to update `ARM_DWT_MAX_DEC_LEVEL`.

## Documentation
All the code is well-documented following Doxygen style.

## Tests
The current fonction have been built with CMSIS 4 using Atollic TrueStudio. The test passes on a stm32h7 (cortex-m7) calling `dwt_test_f32( 1E-4 )` which guarantee that the outputs between `arm_dwt_f32( )` and the PyWavelet library are the same with a relative error of 1e-4.

*Note*: I only did the test on 1 sample

## TODO
- Add the other padding possibilities (for now, only zero and symmetrical padding are supported)
- See if and how `arm_conv_f32` could be used to optimize computation
- Do a short execution time test on a specific hardware
