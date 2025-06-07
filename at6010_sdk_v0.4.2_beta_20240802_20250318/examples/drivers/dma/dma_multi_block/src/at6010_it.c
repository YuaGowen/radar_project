#include "hal_dma.h"

extern dma_handle_t hdmax;

void DMA_Handler(void)
{
	hal_dma_irq_handler(&hdmax);
}
