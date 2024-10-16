static int kvm_lapic_reg_read(struct kvm_lapic *apic, u32 offset, int len,
			      void *data)
{
	unsigned char alignment = offset & 0xf;
	u32 result;
	/* this bitmask has a bit cleared for each reserved register */
	u64 valid_reg_mask =
		APIC_REG_MASK(APIC_ID) |
		APIC_REG_MASK(APIC_LVR) |
		APIC_REG_MASK(APIC_TASKPRI) |
		APIC_REG_MASK(APIC_PROCPRI) |
		APIC_REG_MASK(APIC_LDR) |
		APIC_REG_MASK(APIC_DFR) |
		APIC_REG_MASK(APIC_SPIV) |
		APIC_REGS_MASK(APIC_ISR, APIC_ISR_NR) |
		APIC_REGS_MASK(APIC_TMR, APIC_ISR_NR) |
		APIC_REGS_MASK(APIC_IRR, APIC_ISR_NR) |
		APIC_REG_MASK(APIC_ESR) |
		APIC_REG_MASK(APIC_ICR) |
		APIC_REG_MASK(APIC_LVTT) |
		APIC_REG_MASK(APIC_LVTTHMR) |
		APIC_REG_MASK(APIC_LVTPC) |
		APIC_REG_MASK(APIC_LVT0) |
		APIC_REG_MASK(APIC_LVT1) |
		APIC_REG_MASK(APIC_LVTERR) |
		APIC_REG_MASK(APIC_TMICT) |
		APIC_REG_MASK(APIC_TMCCT) |
		APIC_REG_MASK(APIC_TDCR);

	/*
	 * ARBPRI and ICR2 are not valid in x2APIC mode.  WARN if KVM reads ICR
	 * in x2APIC mode as it's an 8-byte register in x2APIC and needs to be
	 * manually handled by the caller.
	 */
	if (!apic_x2apic_mode(apic))
		valid_reg_mask |= APIC_REG_MASK(APIC_ARBPRI) |
				  APIC_REG_MASK(APIC_ICR2);
	else
		WARN_ON_ONCE(offset == APIC_ICR);

	if (alignment + len > 4)
		return 1;

	if (offset > 0x3f0 || !(valid_reg_mask & APIC_REG_MASK(offset)))
		return 1;

	result = __apic_read(apic, offset & ~0xf);

	trace_kvm_apic_read(offset, result);

	switch (len) {
	case 1:
	case 2:
	case 4:
		memcpy(data, (char *)&result + alignment, len);
		break;
	default:
		printk(KERN_ERR "Local APIC read with len = %x, "
		       "should be 1,2, or 4 instead\n", len);
		break;
	}
	return 0;
}