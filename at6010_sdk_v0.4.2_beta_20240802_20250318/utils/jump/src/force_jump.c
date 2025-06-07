#include <stdio.h>
#include <stdlib.h>
#include "at6010.h"
#include "cache.h"
#include "force_jump.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
#define EnablePrivilegedMode() __asm("SVC #0")


/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/

ATTR_RAM_SECTION static void cache_flush(void)
{
	hwp_cache->config = CONFIG_CACHE_EN(0U);
	hwp_cache->config = CONFIG_CACHE_EN(1U) | CONFIG_CACHE_SIZE(0U) | CONFIG_MISS_CNT_EN(1U) |
	                    CONFIG_HIT_CNT_EN(1U) | CONFIG_CACHE_LINE_NOFLUSH_N(1U);
	hwp_cache->ctrl = CTRL_CACHE_LINE_FLUSH(1U);
	for (u32 i = 0U; i < 1000U; i++) {
		__NOP();
	}
}

__attribute__((naked, noreturn)) void force_jump_asm(unsigned int SP, unsigned int RH)
{
	__asm("MSR      MSP,r0");
	__asm("BX       r1");
}

void force_jump(unsigned int* Address)
{
	//1. Make sure, the CPU is in privileged mode
	if (CONTROL_nPRIV_Msk & __get_CONTROL()) {  /* not in privileged mode */
		EnablePrivilegedMode();
	}

	//2. Disable interrupt response.
	__disable_irq();

	//3. Disable all enabled interrupts in NVIC.
	// memset((uint32_t*)NVIC->ICER, 0xFF, sizeof(NVIC->ICER));
	NVIC->ICER[0] = 0xFFFFFFFF;

	/* 4. Disable all enabled peripherals which might generate interrupt requests.
	*  Clear all pending interrupt flags in those peripherals.
	*  This part is device-dependent, and you can write it by referring to device datasheet.
	*/

	/* 5. Clear all pending interrupt requests in NVIC. */
	// memset((uint32_t*)NVIC->ICPR, 0xFF, sizeof(NVIC->ICPR));
	NVIC->ICPR[0] = 0xFFFFFFFF;

	//6. Disable SysTick and clear its exception pending bit.
	SysTick->CTRL = 0;
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

	//7. Disable individual fault handlers if the bootloader used them
#ifdef SCB_SHCSR_USGFAULTENA_Msk
	SCB->SHCSR &= ~SCB_SHCSR_USGFAULTENA_Msk;
#endif
#ifdef SCB_SHCSR_BUSFAULTENA_Msk
	SCB->SHCSR &= ~SCB_SHCSR_BUSFAULTENA_Msk;
#endif
#ifdef SCB_SHCSR_MEMFAULTENA_Msk
	SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
#endif

	//8. Activate the MSP, if the core is found to currently run with the PSP. As the compiler might still use the stack, the PSP needs to be copied to the MSP before this.
	if (CONTROL_SPSEL_Msk & __get_CONTROL()) {  /* MSP is not active */
		__set_MSP(__get_PSP());
		__set_CONTROL(__get_CONTROL() & ~CONTROL_SPSEL_Msk);
	}

	//9. Load the vector table address of the user application into SCB->VTOR register. Make sure the address meets the alignment requirements.
	SCB->VTOR = (uint32_t)Address;

	//10. call the jump function with the address as a parameter
	cache_flush();
	force_jump_asm(Address[0], Address[1]);
}
