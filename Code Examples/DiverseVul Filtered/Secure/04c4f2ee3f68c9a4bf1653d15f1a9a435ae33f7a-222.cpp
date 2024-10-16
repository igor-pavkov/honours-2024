static u64 vcpu_supported_debugctl(struct kvm_vcpu *vcpu)
{
	u64 debugctl = vmx_supported_debugctl();

	if (!intel_pmu_lbr_is_enabled(vcpu))
		debugctl &= ~DEBUGCTLMSR_LBR_MASK;

	return debugctl;
}