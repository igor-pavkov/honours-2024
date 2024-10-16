static int __init parse_pci(char *arg)
{
	if (arg && strcmp(arg, "noacpi") == 0)
		acpi_disable_pci();
	return 0;
}