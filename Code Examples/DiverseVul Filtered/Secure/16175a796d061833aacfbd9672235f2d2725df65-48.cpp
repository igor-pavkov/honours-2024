static unsigned long vmx_get_rflags(struct kvm_vcpu *vcpu)
{
	return vmcs_readl(GUEST_RFLAGS);
}