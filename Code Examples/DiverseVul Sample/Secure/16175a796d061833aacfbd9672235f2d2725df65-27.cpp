static inline int vm_need_virtualize_apic_accesses(struct kvm *kvm)
{
	return ((cpu_has_vmx_virtualize_apic_accesses()) &&
		(irqchip_in_kernel(kvm)));
}