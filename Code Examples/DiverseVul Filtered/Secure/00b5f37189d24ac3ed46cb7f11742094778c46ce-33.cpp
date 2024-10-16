static __always_inline u64 __kvm_lapic_get_reg64(char *regs, int reg)
{
	BUILD_BUG_ON(reg != APIC_ICR);
	return *((u64 *) (regs + reg));
}