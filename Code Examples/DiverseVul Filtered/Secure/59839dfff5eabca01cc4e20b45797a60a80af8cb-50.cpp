static void kernel_pio(struct kvm_io_device *pio_dev,
		       struct kvm_vcpu *vcpu,
		       void *pd)
{
	/* TODO: String I/O for in kernel device */

	mutex_lock(&vcpu->kvm->lock);
	if (vcpu->arch.pio.in)
		kvm_iodevice_read(pio_dev, vcpu->arch.pio.port,
				  vcpu->arch.pio.size,
				  pd);
	else
		kvm_iodevice_write(pio_dev, vcpu->arch.pio.port,
				   vcpu->arch.pio.size,
				   pd);
	mutex_unlock(&vcpu->kvm->lock);
}