static int apic_has_interrupt_for_ppr(struct kvm_lapic *apic, u32 ppr)
{
	int highest_irr;
	if (kvm_x86_ops.sync_pir_to_irr)
		highest_irr = static_call(kvm_x86_sync_pir_to_irr)(apic->vcpu);
	else
		highest_irr = apic_find_highest_irr(apic);
	if (highest_irr == -1 || (highest_irr & 0xF0) <= ppr)
		return -1;
	return highest_irr;
}