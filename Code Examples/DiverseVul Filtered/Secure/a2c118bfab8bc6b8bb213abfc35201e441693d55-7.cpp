static inline struct kvm_ioapic *to_ioapic(struct kvm_io_device *dev)
{
	return container_of(dev, struct kvm_ioapic, dev);
}