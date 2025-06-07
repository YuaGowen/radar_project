;/**************************************************************************//**
; * @file     startup_CMSDK_CM0plus.s
; * @brief    CMSIS Cortex-M0+ Core Device Startup File for
; *           Device CMSDK_CM0plus
; * @version  V3.01
; * @date     06. March 2012
; *
; * @note
; * Copyright (C) 2012 ARM Limited. All rights reserved.
; *
; * @par
; * ARM Limited (ARM) is supplying this software for use with Cortex-M
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * @par
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/
;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000800

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000100

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler
                DCD     TIMER0_Handler            ; 16+ 0: TIMER 0 Handler         
                DCD     TIMER1_Handler            ; 16+ 1: TIMER 1 Handler         
                DCD     TIMER2_Handler            ; 16+ 2: TIMER 2 Handler         
                DCD     TIMER3_Handler            ; 16+ 3: TIMER 3 Handler         
                DCD     DUALTIMER_Handler         ; 16+ 4: Dual timer 2 Handler    
                DCD     UART0_Handler             ; 16+ 5: UART 0 Handler          
                DCD     UART1_Handler             ; 16+ 6: UART 1 Handler          
                DCD     UART2_Handler             ; 16+ 7: UART 2 Handler          
                DCD     SPI0_Handler              ; 16+ 8: SPI 0 Handler           
                DCD     SPI1_Handler              ; 16+ 9: SPI 1 Handler           
                DCD     I2C_Handler               ; 16+10: I2C Handler             
                DCD     GPIO_Handler              ; 16+11: GPIO Combined Handler   
                DCD     CACHE_Handler             ; 16+12: Cache Combined Handler  
                DCD     DMA_Handler               ; 16+13: DMA Combined Handler    
                DCD     RADAR_Handler             ; 16+14: RADAR Combinaed Handler 
                DCD     RC_CAL_Handler             ; 16+15: Reserve 1 Handler       
                DCD     PLL_Handler               ; 16+16: PLL Lock/Unlock Handler 
                DCD     KEY_Handler               ; 16+17: KEY Handler             
                DCD     EFUSE_Handler             ; 16+18: EFUSE Read/Write Handler
                DCD     AUX_ADC_Handler           ; 16+19: AUX ADC Handler         
                DCD     AFC_PLL_Handler           ; 16+20: AFC PLL Handler         
                DCD     LIGHT_Handler             ; 16+21: Light Handler            
                DCD     LOW_BAT_Handler           ; 16+22: Low Battery Handler       
                DCD     BOR_Handler               ; 16+23: BOR Handler       
                DCD     GPIO0_Handler             ; 16+24: GPIO 0 Handler          
                DCD     GPIO1_Handler             ; 16+25: GPIO 1 Handler          
                DCD     GPIO2_Handler             ; 16+26: GPIO 2 Handler          
                DCD     GPIO3_Handler             ; 16+27: GPIO 3 Handler          
                DCD     GPIO4_Handler             ; 16+28: GPIO 4 Handler          
                DCD     GPIO5_Handler             ; 16+29: GPIO 5 Handler          
                DCD     GPIO6_Handler             ; 16+30: GPIO 6 Handler          
                DCD     AON_WDT_Handler           ; 16+31: AON Watchdog Handler          
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
               EXPORT  SysTick_Handler            [WEAK]
               B       .
               ENDP
Default_Handler PROC
                EXPORT TIMER0_Handler             [WEAK]
                EXPORT TIMER1_Handler             [WEAK]
                EXPORT TIMER2_Handler             [WEAK]
                EXPORT TIMER3_Handler             [WEAK]
                EXPORT DUALTIMER_Handler          [WEAK]
                EXPORT UART0_Handler              [WEAK]
                EXPORT UART1_Handler              [WEAK]
                EXPORT UART2_Handler              [WEAK]
                EXPORT SPI0_Handler               [WEAK]
                EXPORT SPI1_Handler               [WEAK]
                EXPORT I2C_Handler                [WEAK]
                EXPORT GPIO_Handler               [WEAK]
                EXPORT CACHE_Handler              [WEAK]
                EXPORT DMA_Handler                [WEAK]
                EXPORT RADAR_Handler              [WEAK]
                EXPORT RC_CAL_Handler             [WEAK]
                EXPORT PLL_Handler                [WEAK]
                EXPORT KEY_Handler                [WEAK]
                EXPORT EFUSE_Handler              [WEAK]
                EXPORT AUX_ADC_Handler            [WEAK]
                EXPORT AFC_PLL_Handler            [WEAK]
                EXPORT LIGHT_Handler              [WEAK]
                EXPORT LOW_BAT_Handler            [WEAK]
                EXPORT BOR_Handler                [WEAK]
                EXPORT GPIO0_Handler              [WEAK]
                EXPORT GPIO1_Handler              [WEAK]
                EXPORT GPIO2_Handler              [WEAK]
                EXPORT GPIO3_Handler              [WEAK]
                EXPORT GPIO4_Handler              [WEAK]
                EXPORT GPIO5_Handler              [WEAK]
                EXPORT GPIO6_Handler              [WEAK]
                EXPORT AON_WDT_Handler              [WEAK]
TIMER0_Handler   
TIMER1_Handler   
TIMER2_Handler   
TIMER3_Handler   
DUALTIMER_Handler
UART0_Handler    
UART1_Handler    
UART2_Handler    
SPI0_Handler     
SPI1_Handler     
I2C_Handler      
GPIO_Handler     
CACHE_Handler    
DMA_Handler      
RADAR_Handler    
RC_CAL_Handler
PLL_Handler
KEY_Handler
EFUSE_Handler
AUX_ADC_Handler
AFC_PLL_Handler
LIGHT_Handler
LOW_BAT_Handler
BOR_Handler
GPIO0_Handler
GPIO1_Handler
GPIO2_Handler
GPIO3_Handler
GPIO4_Handler
GPIO5_Handler
GPIO6_Handler
AON_WDT_Handler
                B       .
                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END
