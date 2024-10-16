static int __init parse_acpi(char *arg)
{
	if (!arg)
		return -EINVAL;

	/* "acpi=off" disables both ACPI table parsing and interpreter */
	if (strcmp(arg, "off") == 0) {
		disable_acpi();
	}
	/* acpi=force to over-ride black-list */
	else if (strcmp(arg, "force") == 0) {
		acpi_force = 1;
		acpi_disabled = 0;
	}
	/* acpi=strict disables out-of-spec workarounds */
	else if (strcmp(arg, "strict") == 0) {
		acpi_strict = 1;
	}
	/* acpi=rsdt use RSDT instead of XSDT */
	else if (strcmp(arg, "rsdt") == 0) {
		acpi_gbl_do_not_use_xsdt = TRUE;
	}
	/* "acpi=noirq" disables ACPI interrupt routing */
	else if (strcmp(arg, "noirq") == 0) {
		acpi_noirq_set();
	}
	/* "acpi=copy_dsdt" copys DSDT */
	else if (strcmp(arg, "copy_dsdt") == 0) {
		acpi_gbl_copy_dsdt_locally = 1;
	}
	/* "acpi=nocmcff" disables FF mode for corrected errors */
	else if (strcmp(arg, "nocmcff") == 0) {
		acpi_disable_cmcff = 1;
	} else {
		/* Core will printk when we return error. */
		return -EINVAL;
	}
	return 0;
}