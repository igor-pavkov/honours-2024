static int __init acpi_parse_hpet(struct acpi_table_header *table)
{
	struct acpi_table_hpet *hpet_tbl = (struct acpi_table_hpet *)table;

	if (hpet_tbl->address.space_id != ACPI_SPACE_MEM) {
		printk(KERN_WARNING PREFIX "HPET timers must be located in "
		       "memory.\n");
		return -1;
	}

	hpet_address = hpet_tbl->address.address;
	hpet_blockid = hpet_tbl->sequence;

	/*
	 * Some broken BIOSes advertise HPET at 0x0. We really do not
	 * want to allocate a resource there.
	 */
	if (!hpet_address) {
		printk(KERN_WARNING PREFIX
		       "HPET id: %#x base: %#lx is invalid\n",
		       hpet_tbl->id, hpet_address);
		return 0;
	}
#ifdef CONFIG_X86_64
	/*
	 * Some even more broken BIOSes advertise HPET at
	 * 0xfed0000000000000 instead of 0xfed00000. Fix it up and add
	 * some noise:
	 */
	if (hpet_address == 0xfed0000000000000UL) {
		if (!hpet_force_user) {
			printk(KERN_WARNING PREFIX "HPET id: %#x "
			       "base: 0xfed0000000000000 is bogus\n "
			       "try hpet=force on the kernel command line to "
			       "fix it up to 0xfed00000.\n", hpet_tbl->id);
			hpet_address = 0;
			return 0;
		}
		printk(KERN_WARNING PREFIX
		       "HPET id: %#x base: 0xfed0000000000000 fixed up "
		       "to 0xfed00000.\n", hpet_tbl->id);
		hpet_address >>= 32;
	}
#endif
	printk(KERN_INFO PREFIX "HPET id: %#x base: %#lx\n",
	       hpet_tbl->id, hpet_address);

	/*
	 * Allocate and initialize the HPET firmware resource for adding into
	 * the resource tree during the lateinit timeframe.
	 */
#define HPET_RESOURCE_NAME_SIZE 9
	hpet_res = alloc_bootmem(sizeof(*hpet_res) + HPET_RESOURCE_NAME_SIZE);

	hpet_res->name = (void *)&hpet_res[1];
	hpet_res->flags = IORESOURCE_MEM;
	snprintf((char *)hpet_res->name, HPET_RESOURCE_NAME_SIZE, "HPET %u",
		 hpet_tbl->sequence);

	hpet_res->start = hpet_address;
	hpet_res->end = hpet_address + (1 * 1024) - 1;

	return 0;
}