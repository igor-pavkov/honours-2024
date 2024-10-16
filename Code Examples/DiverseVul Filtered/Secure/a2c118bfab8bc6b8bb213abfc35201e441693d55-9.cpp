void kvm_ioapic_update_eoi(struct kvm *kvm, int vector, int trigger_mode)
{
	struct kvm_ioapic *ioapic = kvm->arch.vioapic;

	spin_lock(&ioapic->lock);
	__kvm_ioapic_update_eoi(ioapic, vector, trigger_mode);
	spin_unlock(&ioapic->lock);
}