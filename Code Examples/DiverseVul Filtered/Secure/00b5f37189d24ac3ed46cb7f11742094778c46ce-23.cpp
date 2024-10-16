static inline void apic_clear_irr(int vec, struct kvm_lapic *apic)
{
	struct kvm_vcpu *vcpu;

	vcpu = apic->vcpu;

	if (unlikely(vcpu->arch.apicv_active)) {
		/* need to update RVI */
		kvm_lapic_clear_vector(vec, apic->regs + APIC_IRR);
		static_call_cond(kvm_x86_hwapic_irr_update)(vcpu, apic_find_highest_irr(apic));
	} else {
		apic->irr_pending = false;
		kvm_lapic_clear_vector(vec, apic->regs + APIC_IRR);
		if (apic_search_irr(apic) != -1)
			apic->irr_pending = true;
	}
}