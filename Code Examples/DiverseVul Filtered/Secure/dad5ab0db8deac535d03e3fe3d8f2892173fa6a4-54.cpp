static int __init acpi_parse_sbf(struct acpi_table_header *table)
{
	struct acpi_table_boot *sb = (struct acpi_table_boot *)table;

	sbf_port = sb->cmos_index;	/* Save CMOS port */

	return 0;
}