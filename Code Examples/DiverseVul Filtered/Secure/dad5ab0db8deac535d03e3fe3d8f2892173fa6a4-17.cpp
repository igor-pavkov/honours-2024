static int __init dmi_ignore_irq0_timer_override(const struct dmi_system_id *d)
{
	if (!acpi_skip_timer_override) {
		pr_notice("%s detected: Ignoring BIOS IRQ0 override\n",
			d->ident);
		acpi_skip_timer_override = 1;
	}
	return 0;
}