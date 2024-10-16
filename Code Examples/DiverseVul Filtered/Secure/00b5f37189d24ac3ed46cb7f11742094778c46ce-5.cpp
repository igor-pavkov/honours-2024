static inline void apic_clear_isr(int vec, struct kvm_lapic *apic)
{
	struct kvm_vcpu *vcpu;
	if (!__apic_test_and_clear_vector(vec, apic->regs + APIC_ISR))
		return;

	vcpu = apic->vcpu;

	/*
	 * We do get here for APIC virtualization enabled if the guest
	 * uses the Hyper-V APIC enlightenment.  In this case we may need
	 * to trigger a new interrupt delivery by writing the SVI field;
	 * on the other hand isr_count and highest_isr_cache are unused
	 * and must be left alone.
	 */
	if (unlikely(vcpu->arch.apicv_active))
		static_call_cond(kvm_x86_hwapic_isr_update)(vcpu, apic_find_highest_isr(apic));
	else {
		--apic->isr_count;
		BUG_ON(apic->isr_count < 0);
		apic->highest_isr_cache = -1;
	}
}