static void do_interrupt_requests(struct kvm_vcpu *vcpu,
				       struct kvm_run *kvm_run)
{
	vmx_update_window_states(vcpu);

	if (vcpu->guest_debug & KVM_GUESTDBG_SINGLESTEP)
		vmcs_clear_bits(GUEST_INTERRUPTIBILITY_INFO,
				GUEST_INTR_STATE_STI |
				GUEST_INTR_STATE_MOV_SS);

	if (vcpu->arch.nmi_pending && !vcpu->arch.nmi_injected) {
		if (vcpu->arch.interrupt.pending) {
			enable_nmi_window(vcpu);
		} else if (vcpu->arch.nmi_window_open) {
			vcpu->arch.nmi_pending = false;
			vcpu->arch.nmi_injected = true;
		} else {
			enable_nmi_window(vcpu);
			return;
		}
	}
	if (vcpu->arch.nmi_injected) {
		vmx_inject_nmi(vcpu);
		if (vcpu->arch.nmi_pending)
			enable_nmi_window(vcpu);
		else if (vcpu->arch.irq_summary
			 || kvm_run->request_interrupt_window)
			enable_irq_window(vcpu);
		return;
	}

	if (vcpu->arch.interrupt_window_open) {
		if (vcpu->arch.irq_summary && !vcpu->arch.interrupt.pending)
			kvm_do_inject_irq(vcpu);

		if (vcpu->arch.interrupt.pending)
			vmx_inject_irq(vcpu, vcpu->arch.interrupt.nr);
	}
	if (!vcpu->arch.interrupt_window_open &&
	    (vcpu->arch.irq_summary || kvm_run->request_interrupt_window))
		enable_irq_window(vcpu);
}