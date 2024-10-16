static int vmx_interrupt_allowed(struct kvm_vcpu *vcpu, bool for_injection)
{
	if (to_vmx(vcpu)->nested.nested_run_pending)
		return -EBUSY;

       /*
        * An IRQ must not be injected into L2 if it's supposed to VM-Exit,
        * e.g. if the IRQ arrived asynchronously after checking nested events.
        */
	if (for_injection && is_guest_mode(vcpu) && nested_exit_on_intr(vcpu))
		return -EBUSY;

	return !vmx_interrupt_blocked(vcpu);
}