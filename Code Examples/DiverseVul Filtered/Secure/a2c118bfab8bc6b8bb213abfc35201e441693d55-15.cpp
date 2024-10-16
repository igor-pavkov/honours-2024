void kvm_ioapic_clear_all(struct kvm_ioapic *ioapic, int irq_source_id)
{
	int i;

	spin_lock(&ioapic->lock);
	for (i = 0; i < KVM_IOAPIC_NUM_PINS; i++)
		__clear_bit(irq_source_id, &ioapic->irq_states[i]);
	spin_unlock(&ioapic->lock);
}