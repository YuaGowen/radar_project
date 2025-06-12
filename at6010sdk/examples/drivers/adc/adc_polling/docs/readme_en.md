# Aux ADC sampling -- Polling mode

1. GPIO initializes to analog mode;
2. Aux ADC initialization;
3. Call `adc_get_aux_voltage()` to get a voltage value, or call `adc_get_aux_value()` to get a sample value;
