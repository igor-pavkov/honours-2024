static int vmx_nmi_allowed(struct kvm_vcpu *vcpu, bool for_injection)
{
	if (to_vmx(vcpu)->nested.nested_run_pending)
		return -EBUSY;

	/* An NMI must not be injected into L2 if it's supposed to VM-Exit.  */
	if (for_injection && is_guest_mode(vcpu) && nested_exit_on_nmi(vcpu))
		return -EBUSY;

	return !vmx_nmi_blocked(vcpu);
}