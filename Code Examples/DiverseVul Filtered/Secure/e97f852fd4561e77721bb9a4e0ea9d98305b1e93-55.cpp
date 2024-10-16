static inline bool kvm_vcpu_has_events(struct kvm_vcpu *vcpu)
{
	if (!list_empty_careful(&vcpu->async_pf.done))
		return true;

	if (kvm_apic_has_events(vcpu))
		return true;

	if (vcpu->arch.pv.pv_unhalted)
		return true;

	if (vcpu->arch.exception.pending)
		return true;

	if (kvm_test_request(KVM_REQ_NMI, vcpu) ||
	    (vcpu->arch.nmi_pending &&
	     kvm_x86_ops->nmi_allowed(vcpu)))
		return true;

	if (kvm_test_request(KVM_REQ_SMI, vcpu) ||
	    (vcpu->arch.smi_pending && !is_smm(vcpu)))
		return true;

	if (kvm_arch_interrupt_allowed(vcpu) &&
	    (kvm_cpu_has_interrupt(vcpu) ||
	    kvm_guest_apic_has_interrupt(vcpu)))
		return true;

	if (kvm_hv_has_stimer_pending(vcpu))
		return true;

	return false;
}