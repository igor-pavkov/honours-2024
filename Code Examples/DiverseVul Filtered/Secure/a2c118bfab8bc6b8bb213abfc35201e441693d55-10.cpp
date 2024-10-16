int kvm_ioapic_set_irq(struct kvm_ioapic *ioapic, int irq, int irq_source_id,
		       int level)
{
	u32 old_irr;
	u32 mask = 1 << irq;
	union kvm_ioapic_redirect_entry entry;
	int ret, irq_level;

	BUG_ON(irq < 0 || irq >= IOAPIC_NUM_PINS);

	spin_lock(&ioapic->lock);
	old_irr = ioapic->irr;
	irq_level = __kvm_irq_line_state(&ioapic->irq_states[irq],
					 irq_source_id, level);
	entry = ioapic->redirtbl[irq];
	irq_level ^= entry.fields.polarity;
	if (!irq_level) {
		ioapic->irr &= ~mask;
		ret = 1;
	} else {
		int edge = (entry.fields.trig_mode == IOAPIC_EDGE_TRIG);
		ioapic->irr |= mask;
		if ((edge && old_irr != ioapic->irr) ||
		    (!edge && !entry.fields.remote_irr))
			ret = ioapic_service(ioapic, irq);
		else
			ret = 0; /* report coalesced interrupt */
	}
	trace_kvm_ioapic_set_irq(entry.bits, irq, ret == 0);
	spin_unlock(&ioapic->lock);

	return ret;
}