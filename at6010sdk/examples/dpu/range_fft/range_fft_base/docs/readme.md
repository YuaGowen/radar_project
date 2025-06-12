# INTRODUCTION
This example illustrates how to dump range fft results from radar dsp . The results values are structured as frames which are same as the configured fmcw signal generator. Special values are inserted to each frame as a frame header so as to separate frames.

# FEATURE
- Select output port from uart0/uart1
- Select system clock frequency from 40MHz/80MHz
- ADC sample rate 10MHz
- Range fft 256/512
- sweep bandwith 1Ghz/5GHz

# CONFIGURE
1. cfg_clk.h provides a macro to easily change the system clock and baudrate.
   
- SYSTEM_CLK_80M: select system clock frequency
  - 0: 40000000U
  - 1: 80000000U
2. cfg_radar_hwa.h hardware accelerator-related configurations are provided.
   - FFT_SIZE: fft point,equal to adc chirp size in the  most case
   - FFT_OUT_VALID_RB: valid number of FFT output points
   - FFT_NUM_ADC_CH: the number of adc channels, equal to rx num in the  most case
   - FFT_INPUT_SIZE :fft input data point,equal to chirp size in the  most case
   - FFT_NFRAME: The number of chirp to be processed in each radar frame,equal to frame chirp num in  the most case
   - FFT_DC_CUT_ENABLE: remove dc
   - WIN_ENABLE: fft win enable or disable
3. cfg_radar_profile.h radar profile configure,pls ref radar configure doc
  
# DATAFORMAT
  - eg: fft size 256, output rb :60, chirp num :5
   
|        | chirp1 | chirp2 |chirp3 | chirp4 | chirp5|
|------  | ------ | -------|-----  | -------|---- |
|**rb_0**| c1_rb0 | c2_rb0 |c3_rb0 |c4_rb0  |c5_rb0|
|**rb_1**| c1_rb1 | c2_rb1 |c3_rb1 |c4_rb1  |c5_rb1|
|**rb_2**| c1_rb2 | c2_rb2 |c3_rb2 |c4_rb2  |c5_rb2|
|**rb_3**| c1_rb3 | c2_rb3 |c3_rb3 |c4_rb3  |c5_rb3|
|**...**| ...     | ...    |...    |...     |.....|
|**rb_60**| c1_rb60 | c2_rb60 |c3_rb60 |c4_rb60 |cn_rb60|

**note**: rb:32bit IQ DATA, I:S16,Q:S16

# HOW TO USE
1. Check configure items as above
2. Build the target
3. Connect the select port to USB tools (eg. FTDI)
4. Download and reset the AT6010 board
5. Use PC tools  to capture data

