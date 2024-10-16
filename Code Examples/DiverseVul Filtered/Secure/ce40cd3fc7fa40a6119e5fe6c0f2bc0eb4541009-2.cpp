static inline bool kvm_apic_sw_enabled(struct kvm_lapic *apic)
{
	if (static_key_false(&apic_sw_disabled.key))
		return apic->sw_enabled;
	return true;
}