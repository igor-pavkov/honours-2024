static void __init acpi_reduced_hw_init(void)
{
	if (acpi_gbl_reduced_hardware) {
		/*
		 * Override x86_init functions and bypass legacy pic
		 * in Hardware-reduced ACPI mode
		 */
		x86_init.timers.timer_init	= x86_init_noop;
		x86_init.irqs.pre_vector_init	= x86_init_noop;
		legacy_pic			= &null_legacy_pic;
	}
}