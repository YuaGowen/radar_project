# INTRODUCTION
This example illustrates how to sample raw if adc values using fft built-in dma. The raw adc values are structured as frames which are same as the configured fmcw signal generator. A value 0xFFFFFFFF is inserted to each frame as a frame header so as to separate frames.

# FEATURE
- Select output port from uart0/uart1
- Select system clock frequency from 40MHz/80MHz
- ADC sample rate 10MHz

# CONFIGURE
cfg_clk.h provides a macro to easily change the system clock frequency.
To change the detailed radar parameters, source files rawdata_base_main.c shall be modified. This is only for radar developers who are familiar with how fmcw works.

Configure items as following:
- SYSTEM_CLK_80M: select system clock frequency
  - 0: 40000000U
  - 1: 80000000U
- ADC_OUTPUT_PORT_SEL (output_config.c): select output port
  - 0: uart0, TX pin: M6
  - 1: uart1, TX pin: M2
  - 2: spi, SCK pin: M0, CSN pin: M1, MOSI: M2
- NUM_CHIRP: chirps in a sub-frame
- NUM_FRAME: frames quantity, set to 65536 means forever
- NUM_SAMPLE_PER_CHIRP: sample points quantity in a chirp

# HOW TO USE
1. Check configure items as above
2. Build the target
3. Connect the select port to USB tools (eg. FTDI)
4. Download and reset the AT6010 board
5. Use PC tools to save rawdata
6. Use PC tools to show the wave drawn from raw adc value
