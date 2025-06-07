
#ifndef __IRQFLAGS_H__
#define __IRQFLAGS_H__


/**
 * local_irq_save - Save the current interrupt enable state & disable IRQs
 */
static __inline unsigned long local_irq_save(void)
{
	unsigned long flags = 0;
#if defined ( __CC_ARM )
	__asm {
		MRS     flags, PRIMASK
		CPSID   I
	}
#elif defined ( __GNUC__ )
    __asm volatile ("MRS %0, primask" : "=r" (flags) );
    __asm volatile ("CPSID I");
#elif defined (__clang__)
    __asm volatile ("mrs %0, primask" : "=r" (flags) );
    __asm volatile ("cpsid i"::: "memory");
#endif
    return flags;
}

/**
 * local_irq_enable - Enable IRQs
 */
static __inline void local_irq_enable(void)
{
#if defined ( __CC_ARM )
	__asm {
		CPSIE   I   //Clear primask to enable irq
	}
#elif defined ( __GNUC__ )
    __asm volatile ("CPSIE I");
#elif defined (__clang__)
    __asm volatile ( " cpsie i " ::: "memory");
#endif
}

/**
 * local_irq_disable - Disable IRQs
 */
static __inline void local_irq_disable(void)
{
#if defined ( __CC_ARM )
	__asm {
		CPSID   I   //Set primask to disable irq
	}
#elif defined ( __GNUC__ )
    __asm volatile ("CPSID I");
#elif defined (__clang__)
    __asm volatile ("cpsid i " ::: "memory");
#endif
}

/**
 * local_save_flags - Save the current interrupt enable state.
 */
static __inline unsigned long local_save_flags(void)
{
	unsigned long flags;
#if defined ( __CC_ARM )
	__asm {
		MRS     flags, PRIMASK
	}
#elif defined ( __GNUC__ )
    __asm volatile ("MRS %0, primask" : "=r" (flags) );
#elif defined (__clang__)
   __asm volatile ("mrs %0, primask" : "=r" (flags));
#endif

	return flags;
}

/**
 * local_irq_restore - restore saved IRQ & FIQ state
 */
static __inline void local_irq_restore(unsigned long flags)
{
#if defined ( __CC_ARM )
	__asm {
		MSR     PRIMASK, flags
	}
#elif defined ( __GNUC__ )
    __asm volatile ("MSR primask, %0" : : "r" (flags) : "memory");
#elif defined ( __clang__ )
    __asm volatile ("mrs primask, %0" : : "r" (flags) : "memory");
#endif
}

#endif

