static void sdhci_register_types(void)
{
    type_register_static(&sdhci_sysbus_info);
    type_register_static(&sdhci_bus_info);
    type_register_static(&imx_usdhc_info);
    type_register_static(&sdhci_s3c_info);
}