#include "spi_fullduplex_dma_main.h"

void SPI0_Handler(void)
{
	hal_spi_irq_handler(&spi_handle_dma);
}


void DMA_Handler(void)
{
	hal_dma_irq_handler(&hdma_spirx);
	hal_dma_irq_handler(&hdma_spitx);
}
