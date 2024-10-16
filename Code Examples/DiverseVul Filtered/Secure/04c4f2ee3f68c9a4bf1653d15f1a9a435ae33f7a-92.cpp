bool vmx_nmi_blocked(struct kvm_vcpu *vcpu)
{
	if (is_guest_mode(vcpu) && nested_exit_on_nmi(vcpu))
		return false;

	if (!enable_vnmi && to_vmx(vcpu)->loaded_vmcs->soft_vnmi_blocked)
		return true;

	return (vmcs_read32(GUEST_INTERRUPTIBILITY_INFO) &
		(GUEST_INTR_STATE_MOV_SS | GUEST_INTR_STATE_STI |
		 GUEST_INTR_STATE_NMI));
}