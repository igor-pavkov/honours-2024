static int __init acpi_parse_fadt(struct acpi_table_header *table)
{
	if (!(acpi_gbl_FADT.boot_flags & ACPI_FADT_LEGACY_DEVICES)) {
		pr_debug("ACPI: no legacy devices present\n");
		x86_platform.legacy.devices.pnpbios = 0;
	}

	if (acpi_gbl_FADT.header.revision >= FADT2_REVISION_ID &&
	    !(acpi_gbl_FADT.boot_flags & ACPI_FADT_8042) &&
	    x86_platform.legacy.i8042 != X86_LEGACY_I8042_PLATFORM_ABSENT) {
		pr_debug("ACPI: i8042 controller is absent\n");
		x86_platform.legacy.i8042 = X86_LEGACY_I8042_FIRMWARE_ABSENT;
	}

	if (acpi_gbl_FADT.boot_flags & ACPI_FADT_NO_CMOS_RTC) {
		pr_debug("ACPI: not registering RTC platform device\n");
		x86_platform.legacy.rtc = 0;
	}

#ifdef CONFIG_X86_PM_TIMER
	/* detect the location of the ACPI PM Timer */
	if (acpi_gbl_FADT.header.revision >= FADT2_REVISION_ID) {
		/* FADT rev. 2 */
		if (acpi_gbl_FADT.xpm_timer_block.space_id !=
		    ACPI_ADR_SPACE_SYSTEM_IO)
			return 0;

		pmtmr_ioport = acpi_gbl_FADT.xpm_timer_block.address;
		/*
		 * "X" fields are optional extensions to the original V1.0
		 * fields, so we must selectively expand V1.0 fields if the
		 * corresponding X field is zero.
	 	 */
		if (!pmtmr_ioport)
			pmtmr_ioport = acpi_gbl_FADT.pm_timer_block;
	} else {
		/* FADT rev. 1 */
		pmtmr_ioport = acpi_gbl_FADT.pm_timer_block;
	}
	if (pmtmr_ioport)
		printk(KERN_INFO PREFIX "PM-Timer IO Port: %#x\n",
		       pmtmr_ioport);
#endif
	return 0;
}