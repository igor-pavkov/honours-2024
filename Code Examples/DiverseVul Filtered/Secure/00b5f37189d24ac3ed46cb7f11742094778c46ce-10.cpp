void kvm_apic_send_ipi(struct kvm_lapic *apic, u32 icr_low, u32 icr_high)
{
	struct kvm_lapic_irq irq;

	/* KVM has no delay and should always clear the BUSY/PENDING flag. */
	WARN_ON_ONCE(icr_low & APIC_ICR_BUSY);

	irq.vector = icr_low & APIC_VECTOR_MASK;
	irq.delivery_mode = icr_low & APIC_MODE_MASK;
	irq.dest_mode = icr_low & APIC_DEST_MASK;
	irq.level = (icr_low & APIC_INT_ASSERT) != 0;
	irq.trig_mode = icr_low & APIC_INT_LEVELTRIG;
	irq.shorthand = icr_low & APIC_SHORT_MASK;
	irq.msi_redir_hint = false;
	if (apic_x2apic_mode(apic))
		irq.dest_id = icr_high;
	else
		irq.dest_id = GET_APIC_DEST_FIELD(icr_high);

	trace_kvm_apic_ipi(icr_low, irq.dest_id);

	kvm_irq_delivery_to_apic(apic->vcpu->kvm, apic, &irq, NULL);
}