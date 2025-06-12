# INTRODUCTION
This example illustrates how to use user-fft module to proceed fft calculation. Source data is provided in a form of a data array. The fft results is printed so as to check wether the calculation of user fft is correct.

# FEATURE
- TONE_TEST_CASE
- TONE_TEST_COMPLEX

# CONFIGURE
Configure items as following:
- TONE_TEST_CASE use REAL_DATA type to perform fft calculation
- TONE_TEST_COMPLEX use COMPLEX_DATA type to perform fft calculation, TONE_TEST_CASE should also be defined
- WIN_ENABLE: enable window function
- FFT_SIZE: fft size
- OUT_SIZE: result output size
- OUT_FRAMES: quantity of result output frames

# HOW TO USE
1. Check configure items as above
2. Build the target
3. Connect the UART0 port to USB tools (eg. FTDI)
4. Download and reset the AT6010 board
5. Check the printed results
