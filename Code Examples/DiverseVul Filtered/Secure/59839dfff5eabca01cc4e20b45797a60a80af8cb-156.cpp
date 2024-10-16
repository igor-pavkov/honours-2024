static struct kvm_io_device *vcpu_find_pio_dev(struct kvm_vcpu *vcpu,
					       gpa_t addr, int len,
					       int is_write)
{
	return kvm_io_bus_find_dev(&vcpu->kvm->pio_bus, addr, len, is_write);
}