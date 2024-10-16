void kvm_ioapic_scan_entry(struct kvm_vcpu *vcpu, u64 *eoi_exit_bitmap,
			u32 *tmr)
{
	struct kvm_ioapic *ioapic = vcpu->kvm->arch.vioapic;
	union kvm_ioapic_redirect_entry *e;
	int index;

	spin_lock(&ioapic->lock);
	for (index = 0; index < IOAPIC_NUM_PINS; index++) {
		e = &ioapic->redirtbl[index];
		if (!e->fields.mask &&
			(e->fields.trig_mode == IOAPIC_LEVEL_TRIG ||
			 kvm_irq_has_notifier(ioapic->kvm, KVM_IRQCHIP_IOAPIC,
				 index) || index == RTC_GSI)) {
			if (kvm_apic_match_dest(vcpu, NULL, 0,
				e->fields.dest_id, e->fields.dest_mode)) {
				__set_bit(e->fields.vector,
					(unsigned long *)eoi_exit_bitmap);
				if (e->fields.trig_mode == IOAPIC_LEVEL_TRIG)
					__set_bit(e->fields.vector,
						(unsigned long *)tmr);
			}
		}
	}
	spin_unlock(&ioapic->lock);
}