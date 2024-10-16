static inline u32 kvm_apic_get_reg(struct kvm_lapic *apic, int reg_off)
{
	        return *((u32 *) (apic->regs + reg_off));
}