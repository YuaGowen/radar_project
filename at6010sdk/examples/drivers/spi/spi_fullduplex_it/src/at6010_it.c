#include "spi_fullduplex_it_main.h"

void SPI0_Handler(void)
{
	hal_spi_irq_handler(&spi_handle_it);
}
