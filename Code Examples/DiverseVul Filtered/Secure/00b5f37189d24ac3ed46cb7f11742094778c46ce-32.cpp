void kvm_apic_update_apicv(struct kvm_vcpu *vcpu)
{
	struct kvm_lapic *apic = vcpu->arch.apic;

	if (vcpu->arch.apicv_active) {
		/* irr_pending is always true when apicv is activated. */
		apic->irr_pending = true;
		apic->isr_count = 1;
	} else {
		/*
		 * Don't clear irr_pending, searching the IRR can race with
		 * updates from the CPU as APICv is still active from hardware's
		 * perspective.  The flag will be cleared as appropriate when
		 * KVM injects the interrupt.
		 */
		apic->isr_count = count_vectors(apic->regs + APIC_ISR);
	}
}