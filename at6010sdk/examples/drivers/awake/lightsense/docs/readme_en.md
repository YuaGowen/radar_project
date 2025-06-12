# luminance detection

1. Clock initialization
2. GPIO initialization, set to analog mode;
3. Aux ADC initialization;
4. Light sensor initialization, set threshold, set detection interval time, then enable;
5. Call the function `light_get_status()` to get the detection result;
