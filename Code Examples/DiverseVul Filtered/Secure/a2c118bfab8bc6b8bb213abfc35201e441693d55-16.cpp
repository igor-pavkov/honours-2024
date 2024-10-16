void kvm_ioapic_destroy(struct kvm *kvm)
{
	struct kvm_ioapic *ioapic = kvm->arch.vioapic;

	if (ioapic) {
		kvm_io_bus_unregister_dev(kvm, KVM_MMIO_BUS, &ioapic->dev);
		kvm->arch.vioapic = NULL;
		kfree(ioapic);
	}
}