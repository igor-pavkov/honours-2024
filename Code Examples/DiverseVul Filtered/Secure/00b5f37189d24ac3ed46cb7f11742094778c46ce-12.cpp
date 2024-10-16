static __always_inline void __kvm_lapic_set_reg64(char *regs, int reg, u64 val)
{
	BUILD_BUG_ON(reg != APIC_ICR);
	*((u64 *) (regs + reg)) = val;
}