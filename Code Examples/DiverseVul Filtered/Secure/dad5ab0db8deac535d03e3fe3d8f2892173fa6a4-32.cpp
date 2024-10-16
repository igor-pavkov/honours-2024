static int __init parse_acpi_skip_timer_override(char *arg)
{
	acpi_skip_timer_override = 1;
	return 0;
}