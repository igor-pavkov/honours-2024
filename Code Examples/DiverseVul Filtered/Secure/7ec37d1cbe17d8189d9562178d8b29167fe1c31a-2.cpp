static bool kvm_hv_is_syndbg_enabled(struct kvm_vcpu *vcpu)
{
	struct kvm_vcpu_hv *hv_vcpu = to_hv_vcpu(vcpu);

	return hv_vcpu->cpuid_cache.syndbg_cap_eax &
		HV_X64_SYNDBG_CAP_ALLOW_KERNEL_DEBUGGING;
}