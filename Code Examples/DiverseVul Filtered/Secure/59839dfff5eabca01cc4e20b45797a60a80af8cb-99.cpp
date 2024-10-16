static struct kvm_io_device *vcpu_find_mmio_dev(struct kvm_vcpu *vcpu,
						gpa_t addr, int len,
						int is_write)
{
	struct kvm_io_device *dev;

	dev = vcpu_find_pervcpu_dev(vcpu, addr, len, is_write);
	if (dev == NULL)
		dev = kvm_io_bus_find_dev(&vcpu->kvm->mmio_bus, addr, len,
					  is_write);
	return dev;
}