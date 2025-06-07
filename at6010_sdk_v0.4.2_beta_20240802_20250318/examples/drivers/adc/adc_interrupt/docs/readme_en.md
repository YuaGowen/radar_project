# Aux ADC sampling with Interrupt mode

1. Configure a GPIO pin for analog mode;
2. Register Aux ADC interrupt service function;
3. Start Aux ADC continuous conversion;
4. Turn on `AUX_ADC_IRQn` interrupt;
5. Ensure that the Aux ADC interrupt service program is implemented as follows

```c
void AUX_ADC_Handler(void)
{
	adc_aux_isr();
}
```
6. Get the conversion value in the registered Aux ADC interrupt service function and mark the completion of the conversion and stop the Aux ADC interrupt after getting enough sample values;
7. Call application-specific functions to process the sampled data;
