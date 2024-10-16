static void pio_string_write(struct kvm_io_device *pio_dev,
			     struct kvm_vcpu *vcpu)
{
	struct kvm_pio_request *io = &vcpu->arch.pio;
	void *pd = vcpu->arch.pio_data;
	int i;

	mutex_lock(&vcpu->kvm->lock);
	for (i = 0; i < io->cur_count; i++) {
		kvm_iodevice_write(pio_dev, io->port,
				   io->size,
				   pd);
		pd += io->size;
	}
	mutex_unlock(&vcpu->kvm->lock);
}