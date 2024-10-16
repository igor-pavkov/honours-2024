static int acpi_register_gsi_ioapic(struct device *dev, u32 gsi,
				    int trigger, int polarity)
{
	int irq = gsi;
#ifdef CONFIG_X86_IO_APIC
	int node;
	struct irq_alloc_info info;

	node = dev ? dev_to_node(dev) : NUMA_NO_NODE;
	trigger = trigger == ACPI_EDGE_SENSITIVE ? 0 : 1;
	polarity = polarity == ACPI_ACTIVE_HIGH ? 0 : 1;
	ioapic_set_alloc_attr(&info, node, trigger, polarity);

	mutex_lock(&acpi_ioapic_lock);
	irq = mp_map_gsi_to_irq(gsi, IOAPIC_MAP_ALLOC, &info);
	/* Don't set up the ACPI SCI because it's already set up */
	if (irq >= 0 && enable_update_mptable &&
	    acpi_gbl_FADT.sci_interrupt != gsi)
		mp_config_acpi_gsi(dev, gsi, trigger, polarity);
	mutex_unlock(&acpi_ioapic_lock);
#endif

	return irq;
}