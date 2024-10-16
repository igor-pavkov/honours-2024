void kvm_ioapic_calculate_eoi_exitmap(struct kvm_vcpu *vcpu,
					u64 *eoi_exit_bitmap)
{
	struct kvm_ioapic *ioapic = vcpu->kvm->arch.vioapic;
	union kvm_ioapic_redirect_entry *e;
	struct kvm_lapic_irq irqe;
	int index;

	spin_lock(&ioapic->lock);
	/* traverse ioapic entry to set eoi exit bitmap*/
	for (index = 0; index < IOAPIC_NUM_PINS; index++) {
		e = &ioapic->redirtbl[index];
		if (!e->fields.mask &&
			(e->fields.trig_mode == IOAPIC_LEVEL_TRIG ||
			 kvm_irq_has_notifier(ioapic->kvm, KVM_IRQCHIP_IOAPIC,
				 index))) {
			irqe.dest_id = e->fields.dest_id;
			irqe.vector = e->fields.vector;
			irqe.dest_mode = e->fields.dest_mode;
			irqe.delivery_mode = e->fields.delivery_mode << 8;
			kvm_calculate_eoi_exitmap(vcpu, &irqe, eoi_exit_bitmap);
		}
	}
	spin_unlock(&ioapic->lock);
}