int __init acpi_mps_check(void)
{
#if defined(CONFIG_X86_LOCAL_APIC) && !defined(CONFIG_X86_MPPARSE)
/* mptable code is not built-in*/
	if (acpi_disabled || acpi_noirq) {
		printk(KERN_WARNING "MPS support code is not built-in.\n"
		       "Using acpi=off or acpi=noirq or pci=noacpi "
		       "may have problem\n");
		return 1;
	}
#endif
	return 0;
}