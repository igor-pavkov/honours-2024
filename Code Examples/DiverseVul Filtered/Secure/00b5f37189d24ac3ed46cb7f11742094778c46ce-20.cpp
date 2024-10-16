static int kvm_lapic_msr_read(struct kvm_lapic *apic, u32 reg, u64 *data)
{
	u32 low;

	if (reg == APIC_ICR) {
		*data = kvm_lapic_get_reg64(apic, APIC_ICR);
		return 0;
	}

	if (kvm_lapic_reg_read(apic, reg, 4, &low))
		return 1;

	*data = low;

	return 0;
}