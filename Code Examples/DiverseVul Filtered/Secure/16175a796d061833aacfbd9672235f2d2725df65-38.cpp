static inline int vm_need_tpr_shadow(struct kvm *kvm)
{
	return ((cpu_has_vmx_tpr_shadow()) && (irqchip_in_kernel(kvm)));
}