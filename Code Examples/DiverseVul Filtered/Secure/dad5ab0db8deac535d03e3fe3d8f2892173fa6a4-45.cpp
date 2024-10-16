static int __init parse_acpi_use_timer_override(char *arg)
{
	acpi_use_timer_override = 1;
	return 0;
}