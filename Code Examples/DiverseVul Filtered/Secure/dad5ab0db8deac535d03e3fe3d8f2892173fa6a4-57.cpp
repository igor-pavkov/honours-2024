static int mp_config_acpi_gsi(struct device *dev, u32 gsi, int trigger,
			int polarity)
{
#ifdef CONFIG_X86_MPPARSE
	struct mpc_intsrc mp_irq;
	struct pci_dev *pdev;
	unsigned char number;
	unsigned int devfn;
	int ioapic;
	u8 pin;

	if (!acpi_ioapic)
		return 0;
	if (!dev || !dev_is_pci(dev))
		return 0;

	pdev = to_pci_dev(dev);
	number = pdev->bus->number;
	devfn = pdev->devfn;
	pin = pdev->pin;
	/* print the entry should happen on mptable identically */
	mp_irq.type = MP_INTSRC;
	mp_irq.irqtype = mp_INT;
	mp_irq.irqflag = (trigger == ACPI_EDGE_SENSITIVE ? 4 : 0x0c) |
				(polarity == ACPI_ACTIVE_HIGH ? 1 : 3);
	mp_irq.srcbus = number;
	mp_irq.srcbusirq = (((devfn >> 3) & 0x1f) << 2) | ((pin - 1) & 3);
	ioapic = mp_find_ioapic(gsi);
	mp_irq.dstapic = mpc_ioapic_id(ioapic);
	mp_irq.dstirq = mp_find_ioapic_pin(ioapic, gsi);

	mp_save_irq(&mp_irq);
#endif
	return 0;
}