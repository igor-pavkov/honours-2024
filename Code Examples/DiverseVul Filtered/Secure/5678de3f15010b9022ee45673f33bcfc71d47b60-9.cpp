void kvm_rtc_eoi_tracking_restore_one(struct kvm_vcpu *vcpu)
{
	struct kvm_ioapic *ioapic = vcpu->kvm->arch.vioapic;

	spin_lock(&ioapic->lock);
	__rtc_irq_eoi_tracking_restore_one(vcpu);
	spin_unlock(&ioapic->lock);
}