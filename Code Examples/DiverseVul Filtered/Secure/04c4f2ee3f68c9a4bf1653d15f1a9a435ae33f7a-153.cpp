bool vmx_get_nmi_mask(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	bool masked;

	if (!enable_vnmi)
		return vmx->loaded_vmcs->soft_vnmi_blocked;
	if (vmx->loaded_vmcs->nmi_known_unmasked)
		return false;
	masked = vmcs_read32(GUEST_INTERRUPTIBILITY_INFO) & GUEST_INTR_STATE_NMI;
	vmx->loaded_vmcs->nmi_known_unmasked = !masked;
	return masked;
}