bool vmx_interrupt_blocked(struct kvm_vcpu *vcpu)
{
	if (is_guest_mode(vcpu) && nested_exit_on_intr(vcpu))
		return false;

	return !(vmx_get_rflags(vcpu) & X86_EFLAGS_IF) ||
	       (vmcs_read32(GUEST_INTERRUPTIBILITY_INFO) &
		(GUEST_INTR_STATE_STI | GUEST_INTR_STATE_MOV_SS));
}