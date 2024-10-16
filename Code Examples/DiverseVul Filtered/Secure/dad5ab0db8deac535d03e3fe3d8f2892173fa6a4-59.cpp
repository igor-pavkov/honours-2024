int acpi_register_gsi(struct device *dev, u32 gsi, int trigger, int polarity)
{
	return __acpi_register_gsi(dev, gsi, trigger, polarity);
}