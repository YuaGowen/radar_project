# INTRODUCTION
This example illustrates how to dump doppler fft and cfar results from radar dsp . The results values are structured as frames which are same as the configured fmcw signal generator. Special values are inserted to each frame as a frame header so as to separate frames.

# FEATURE
- Select output port from uart0/uart1
- Select system clock frequency from 40MHz/80MHz
- ADC sample rate 10MHz
- Range fft 256/512
- Doppler fft 64/128
- cfar ca-cfar
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
   - FFT_DC_CUT_ENABLE: remove dc offset
   - FFT_OUT_IS_POWER: Power Spectrum |X(k)|^2 or complex data |X(k)| 
   - FFT_NUM_ADC_CH: the number of adc channels, equal to rx num in the  most case
   - FFT_INPUT_SIZE :fft input data point,equal to chirp size in the  most case
   - WIN_ENABLE: fft win enable or disable
   - CA_CFAR_ENABLE：cfar enable
   - CFAR_THR_ABS :cfar absolute threshold, cfar process only if data > CFAR_THR_ABS
   - CFAR_VEL_BOUNDARY_PADDING:velocity cfar padding mode, mirror or cyclic
   - CFAR_VEL_GUARD_CELL:velocity cfar guard cell
   - CFAR_VEL_REF_CELL:velocity cfar ref cell,inclue guard cell, total ref cell = left cell (ref cell - guard cell) + right cell (ref cell - guard cell)
   - CFAR_VEL_THR_FACTOR:velocity cfar threshold factor, cell value > (sum(total_ref_cell_value)/total_ref_cell * CFAR_VEL_THR_FACTOR/2) 
   - CFAR_RGE_BOUNDARY_PADDING: range cfar padding mode, mirror or cyclic 
   - CFAR_RGE_GUARD_CELL:range cfar ref cell,inclue guard cell, total ref cell = left cell (ref cell - guard cell) + right cell (ref cell - guard cell)
   - CFAR_RGE_THR_FACTOR: range cfar threshold factor, cfar alarm if cell value > (sum(total_ref_cell_value)/total_ref_cell * CFAR_VEL_THR_FACTOR/2)
   - CFAR_RGE_THR_VB0: doppler fft idx0 threshold factor, if cell idx = idx0 ,set CFAR_RGE_THR_VB0 to ranget cfar threshold factor


3. cfg_radar_profile.h radar profile configure,pls ref radar configure doc
  
# DATAFORMAT
  - eg: rangefft: 512, dopplerfft: 64  valid rb: 60, chirp num :64 cfar:ca_cfar
   
|        | v0   | v1   |v2   | v3  | ...| v63|
|------  | -----| -----|-----| --- |----|---- |
|**r0**  | vb0  | vb1  |vb3  |vb4  | ...|vb63|
|**r1**  | vb0  | vb1  |vb3  |vb4  | ...|vb63|
|**r2**  | vb0  | vb1  |vb3  |vb4  | ...|vb63|
|**r3**  | vb0  | vb1  |vb3  |vb4  | ...|vb63|
|**...** | ...     | ...    |...   |...|.....|
|**r60** | vb0  | vb1  |vb3  |vb4  | ...|vb63|

dopper fft map to cfar bitmap 

|        | bitmap0                | bitmap1               |
|------  | -----                  |   ----                |
|**r0**  | bit0_vb0 ...bit31_vb31 | bit0_vb32...bit31_vb63|
|**r1**  | bit0_vb0 ...bit31_vb31 | bit0_vb32...bit31_vb63|
|**r2**  | bit0_vb0 ...bit31_vb31 | bit0_vb32...bit31_vb63|
|**r3**  | bit0_vb0 ...bit31_vb31 | bit0_vb32...bit31_vb63|
|**...** | bit0_vb0 ...bit31_vb31 | bit0_vb32...bit31_vb63|
|**r60** | bit0_vb0 ...bit31_vb31 | bit0_vb32...bit31_vb63|

**note**: bitmap: 32bit

# HOW TO USE
1. Check configure items as above
2. Build the target
3. Connect the select port to USB tools (eg. FTDI)
4. Download and reset the AT6010 board
5. Use PC tools  to capture data

