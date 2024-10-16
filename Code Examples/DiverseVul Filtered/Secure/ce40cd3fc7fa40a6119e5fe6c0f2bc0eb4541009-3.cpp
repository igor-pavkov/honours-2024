static inline bool kvm_apic_vid_enabled(struct kvm *kvm)
{
	return kvm_x86_ops->vm_has_apicv(kvm);
}