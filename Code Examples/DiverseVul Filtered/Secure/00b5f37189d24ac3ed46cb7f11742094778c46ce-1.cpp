static inline void kvm_lapic_set_reg(struct kvm_lapic *apic, int reg_off, u32 val)
{
	__kvm_lapic_set_reg(apic->regs, reg_off, val);
}