static int vmx_sync_pir_to_irr(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int max_irr;
	bool max_irr_updated;

	WARN_ON(!vcpu->arch.apicv_active);
	if (pi_test_on(&vmx->pi_desc)) {
		pi_clear_on(&vmx->pi_desc);
		/*
		 * IOMMU can write to PID.ON, so the barrier matters even on UP.
		 * But on x86 this is just a compiler barrier anyway.
		 */
		smp_mb__after_atomic();
		max_irr_updated =
			kvm_apic_update_irr(vcpu, vmx->pi_desc.pir, &max_irr);

		/*
		 * If we are running L2 and L1 has a new pending interrupt
		 * which can be injected, we should re-evaluate
		 * what should be done with this new L1 interrupt.
		 * If L1 intercepts external-interrupts, we should
		 * exit from L2 to L1. Otherwise, interrupt should be
		 * delivered directly to L2.
		 */
		if (is_guest_mode(vcpu) && max_irr_updated) {
			if (nested_exit_on_intr(vcpu))
				kvm_vcpu_exiting_guest_mode(vcpu);
			else
				kvm_make_request(KVM_REQ_EVENT, vcpu);
		}
	} else {
		max_irr = kvm_lapic_find_highest_irr(vcpu);
	}
	vmx_hwapic_irr_update(vcpu, max_irr);
	return max_irr;
}