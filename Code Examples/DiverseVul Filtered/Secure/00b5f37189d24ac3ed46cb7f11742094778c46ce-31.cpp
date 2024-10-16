static __always_inline u64 kvm_lapic_get_reg64(struct kvm_lapic *apic, int reg)
{
	return __kvm_lapic_get_reg64(apic->regs, reg);
}