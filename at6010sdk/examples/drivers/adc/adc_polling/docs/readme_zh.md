# Aux ADC 采样 -- 轮询模式

1. GPIO 初始化为模拟模式；
2. Aux ADC 初始化；
3. 调用 `adc_get_aux_voltage()` 获取电压值，或者调用 `adc_get_aux_value()` 获取采样值；
