static void vmx_intr_assist(struct kvm_vcpu *vcpu)
{
	update_tpr_threshold(vcpu);

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
		else if (kvm_cpu_has_interrupt(vcpu))
			enable_irq_window(vcpu);
		return;
	}
	if (!vcpu->arch.interrupt.pending && kvm_cpu_has_interrupt(vcpu)) {
		if (vcpu->arch.interrupt_window_open)
			kvm_queue_interrupt(vcpu, kvm_cpu_get_interrupt(vcpu));
		else
			enable_irq_window(vcpu);
	}
	if (vcpu->arch.interrupt.pending) {
		vmx_inject_irq(vcpu, vcpu->arch.interrupt.nr);
		if (kvm_cpu_has_interrupt(vcpu))
			enable_irq_window(vcpu);
	}
}