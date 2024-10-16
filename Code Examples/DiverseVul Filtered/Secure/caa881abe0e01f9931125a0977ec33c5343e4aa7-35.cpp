static void pxa2xx_register_types(void)
{
    type_register_static(&pxa2xx_i2c_slave_info);
    type_register_static(&pxa2xx_ssp_info);
    type_register_static(&pxa2xx_i2c_info);
    type_register_static(&pxa2xx_rtc_sysbus_info);
}