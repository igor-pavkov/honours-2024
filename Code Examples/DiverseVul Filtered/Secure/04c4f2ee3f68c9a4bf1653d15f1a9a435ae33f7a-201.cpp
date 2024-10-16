static inline bool guest_inject_ac(struct kvm_vcpu *vcpu)
{
	if (!boot_cpu_has(X86_FEATURE_SPLIT_LOCK_DETECT))
		return true;

	return vmx_get_cpl(vcpu) == 3 && kvm_read_cr0_bits(vcpu, X86_CR0_AM) &&
	       (kvm_get_rflags(vcpu) & X86_EFLAGS_AC);
}