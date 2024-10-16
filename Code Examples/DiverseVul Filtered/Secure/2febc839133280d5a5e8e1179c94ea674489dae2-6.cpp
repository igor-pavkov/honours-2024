static inline struct kvm_pit *dev_to_pit(struct kvm_io_device *dev)
{
	return container_of(dev, struct kvm_pit, dev);
}