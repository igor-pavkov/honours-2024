static int kvm_lapic_msr_write(struct kvm_lapic *apic, u32 reg, u64 data)
{
	/*
	 * ICR is a 64-bit register in x2APIC mode (and Hyper'v PV vAPIC) and
	 * can be written as such, all other registers remain accessible only
	 * through 32-bit reads/writes.
	 */
	if (reg == APIC_ICR)
		return kvm_x2apic_icr_write(apic, data);

	return kvm_lapic_reg_write(apic, reg, (u32)data);
}