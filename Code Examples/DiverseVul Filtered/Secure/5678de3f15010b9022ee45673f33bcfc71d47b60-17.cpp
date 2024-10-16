static void rtc_irq_eoi(struct kvm_ioapic *ioapic, struct kvm_vcpu *vcpu)
{
	if (test_and_clear_bit(vcpu->vcpu_id, ioapic->rtc_status.dest_map))
		--ioapic->rtc_status.pending_eoi;

	WARN_ON(ioapic->rtc_status.pending_eoi < 0);
}