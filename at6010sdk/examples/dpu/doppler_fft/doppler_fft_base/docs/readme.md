# INTRODUCTION
This example illustrates how to dump doppler fft results from radar dsp . The results values are structured as frames which are same as the configured fmcw signal generator. Special values are inserted to each frame as a frame header so as to separate frames.

# FEATURE
- Select output port from uart0/uart1
- Select system clock frequency from 40MHz/80MHz
- ADC sample rate 10MHz
- Range fft 256/512
- Doppler fft 64/128
- sweep bandwith 1Ghz/5GHz

# CONFIGURE
1. cfg_clk.h provides a macro to easily change the system clock and baudrate.
   
- SYSTEM_CLK_80M: select system clock frequency
  - 0: 40000000U
  - 1: 80000000U
2. cfg_radar_hwa.h hardware accelerator-related configurations are provided.
   - FFT_RSIZE: range fft point,equal to adc chirp size in the  most case
   - FFT_VSIZE: doppler fft point,equal to frame chirp num in the  most case
   - FFT_OUT_VALID_RB: valid number of FFT output points
   - FFT_OUT_VALID_VB: always equal to FFT_VSIZE
   - FFT_NUM_ADC_CH: the number of adc channels, equal to rx num in the  most case
   - FFT_INPUT_SIZE :fft input data point,equal to chirp size in the  most case
   - WIN_ENABLE: fft win enable or disable
   - FFT_DC_CUT_ENABLE：remove dc 
   - FFT_OUT_IS_POWER： Power Spectrum or complex data
3. cfg_radar_profile.h radar profile configure,pls ref radar configure doc
  
# DATAFORMAT
  - eg: rangefft: 256, dopplerfft: 64  valid rb: 60, chirp num :64
   
|        | v0   | v1   |v2   | v3  | ...| v63|
|------  | -----| -----|-----| --- |----|---- |
|**r0**  | vb0  | vb1  |vb3  |vb4  | ...|vb63|
|**r1**  | vb0  | vb1  |vb3  |vb4  | ...|vb63|
|**r2**  | vb0  | vb1  |vb3  |vb4  | ...|vb63|
|**r3**  | vb0  | vb1  |vb3  |vb4  | ...|vb63|
|**...** | ...     | ...    |...   |...|.....|
|**r60** | vb0  | vb1  |vb3  |vb4  | ...|vb63|

**note**: vb:32bit IQ DATA, I:S16,Q:S16

# HOW TO USE
1. Check configure items as above
2. Build the target
3. Connect the select port to USB tools (eg. FTDI)
4. Download and reset the AT6010 board
5. Use PC tools  to capture data

