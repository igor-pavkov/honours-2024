static void __init mp_config_acpi_legacy_irqs(void)
{
	int i;
	struct mpc_intsrc mp_irq;

#ifdef CONFIG_EISA
	/*
	 * Fabricate the legacy ISA bus (bus #31).
	 */
	mp_bus_id_to_type[MP_ISA_BUS] = MP_BUS_ISA;
#endif
	set_bit(MP_ISA_BUS, mp_bus_not_pci);
	pr_debug("Bus #%d is ISA\n", MP_ISA_BUS);

	/*
	 * Use the default configuration for the IRQs 0-15.  Unless
	 * overridden by (MADT) interrupt source override entries.
	 */
	for (i = 0; i < nr_legacy_irqs(); i++) {
		int ioapic, pin;
		unsigned int dstapic;
		int idx;
		u32 gsi;

		/* Locate the gsi that irq i maps to. */
		if (acpi_isa_irq_to_gsi(i, &gsi))
			continue;

		/*
		 * Locate the IOAPIC that manages the ISA IRQ.
		 */
		ioapic = mp_find_ioapic(gsi);
		if (ioapic < 0)
			continue;
		pin = mp_find_ioapic_pin(ioapic, gsi);
		dstapic = mpc_ioapic_id(ioapic);

		for (idx = 0; idx < mp_irq_entries; idx++) {
			struct mpc_intsrc *irq = mp_irqs + idx;

			/* Do we already have a mapping for this ISA IRQ? */
			if (irq->srcbus == MP_ISA_BUS && irq->srcbusirq == i)
				break;

			/* Do we already have a mapping for this IOAPIC pin */
			if (irq->dstapic == dstapic && irq->dstirq == pin)
				break;
		}

		if (idx != mp_irq_entries) {
			printk(KERN_DEBUG "ACPI: IRQ%d used by override.\n", i);
			continue;	/* IRQ already used */
		}

		mp_irq.type = MP_INTSRC;
		mp_irq.irqflag = 0;	/* Conforming */
		mp_irq.srcbus = MP_ISA_BUS;
		mp_irq.dstapic = dstapic;
		mp_irq.irqtype = mp_INT;
		mp_irq.srcbusirq = i; /* Identity mapped */
		mp_irq.dstirq = pin;

		mp_save_irq(&mp_irq);
	}
}