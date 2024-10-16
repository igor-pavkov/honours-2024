static struct kvm_io_device *vcpu_find_pervcpu_dev(struct kvm_vcpu *vcpu,
						gpa_t addr, int len,
						int is_write)
{
	struct kvm_io_device *dev;

	if (vcpu->arch.apic) {
		dev = &vcpu->arch.apic->dev;
		if (dev->in_range(dev, addr, len, is_write))
			return dev;
	}
	return NULL;
}