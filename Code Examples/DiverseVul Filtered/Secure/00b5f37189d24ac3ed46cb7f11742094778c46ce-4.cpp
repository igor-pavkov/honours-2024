static inline void __kvm_lapic_set_reg(char *regs, int reg_off, u32 val)
{
	*((u32 *) (regs + reg_off)) = val;
}