# Aux ADC 采样 -- 中断模式

1. 配置某个 GPIO 管脚为模拟方式；
2. 注册 Aux ADC 中断服务函数；
3. 启动 Aux ADC 连续转换；
4. 开启 `AUX_ADC_IRQn` 中断；
5. 确保 Aux ADC 中断服务程序调用如下

```c
void AUX_ADC_Handler(void)
{
	adc_aux_isr();
}
```
6. 在注册的 Aux ADC 中断服务函数中获取转换值，并在取得足够的采样值之后标记转换完成，并停止 Aux ADC 中断；
7. 调用应用特定的函数处理采样数据；
