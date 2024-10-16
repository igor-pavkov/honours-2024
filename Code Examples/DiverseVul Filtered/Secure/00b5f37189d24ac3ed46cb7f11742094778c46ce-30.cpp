static __always_inline void kvm_lapic_set_reg64(struct kvm_lapic *apic,
						int reg, u64 val)
{
	__kvm_lapic_set_reg64(apic->regs, reg, val);
}