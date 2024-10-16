static inline int kvm_apic_hw_enabled(struct kvm_lapic *apic)
{
	if (static_key_false(&apic_hw_disabled.key))
		return apic->vcpu->arch.apic_base & MSR_IA32_APICBASE_ENABLE;
	return MSR_IA32_APICBASE_ENABLE;
}