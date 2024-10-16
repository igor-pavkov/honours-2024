void kvm_lapic_exit(void)
{
	static_key_deferred_flush(&apic_hw_disabled);
	WARN_ON(static_branch_unlikely(&apic_hw_disabled.key));
	static_key_deferred_flush(&apic_sw_disabled);
	WARN_ON(static_branch_unlikely(&apic_sw_disabled.key));
}