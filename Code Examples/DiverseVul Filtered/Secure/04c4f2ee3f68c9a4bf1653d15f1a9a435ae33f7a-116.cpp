static u64 nested_vmx_truncate_sysenter_addr(struct kvm_vcpu *vcpu,
						    u64 data)
{
#ifdef CONFIG_X86_64
	if (!guest_cpuid_has(vcpu, X86_FEATURE_LM))
		return (u32)data;
#endif
	return (unsigned long)data;
}