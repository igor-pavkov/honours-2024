void vmx_set_intercept_for_msr(struct kvm_vcpu *vcpu,
						      u32 msr, int type, bool value)
{
	if (value)
		vmx_enable_intercept_for_msr(vcpu, msr, type);
	else
		vmx_disable_intercept_for_msr(vcpu, msr, type);
}