int kvm_set_ioapic(struct kvm *kvm, struct kvm_ioapic_state *state)
{
	struct kvm_ioapic *ioapic = ioapic_irqchip(kvm);
	if (!ioapic)
		return -EINVAL;

	spin_lock(&ioapic->lock);
	memcpy(ioapic, state, sizeof(struct kvm_ioapic_state));
	update_handled_vectors(ioapic);
	kvm_ioapic_make_eoibitmap_request(kvm);
	spin_unlock(&ioapic->lock);
	return 0;
}