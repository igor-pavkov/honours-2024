int kvm_emulate_pio(struct kvm_vcpu *vcpu, struct kvm_run *run, int in,
		  int size, unsigned port)
{
	struct kvm_io_device *pio_dev;
	unsigned long val;

	vcpu->run->exit_reason = KVM_EXIT_IO;
	vcpu->run->io.direction = in ? KVM_EXIT_IO_IN : KVM_EXIT_IO_OUT;
	vcpu->run->io.size = vcpu->arch.pio.size = size;
	vcpu->run->io.data_offset = KVM_PIO_PAGE_OFFSET * PAGE_SIZE;
	vcpu->run->io.count = vcpu->arch.pio.count = vcpu->arch.pio.cur_count = 1;
	vcpu->run->io.port = vcpu->arch.pio.port = port;
	vcpu->arch.pio.in = in;
	vcpu->arch.pio.string = 0;
	vcpu->arch.pio.down = 0;
	vcpu->arch.pio.rep = 0;

	if (vcpu->run->io.direction == KVM_EXIT_IO_IN)
		KVMTRACE_2D(IO_READ, vcpu, vcpu->run->io.port, (u32)size,
			    handler);
	else
		KVMTRACE_2D(IO_WRITE, vcpu, vcpu->run->io.port, (u32)size,
			    handler);

	val = kvm_register_read(vcpu, VCPU_REGS_RAX);
	memcpy(vcpu->arch.pio_data, &val, 4);

	pio_dev = vcpu_find_pio_dev(vcpu, port, size, !in);
	if (pio_dev) {
		kernel_pio(pio_dev, vcpu, vcpu->arch.pio_data);
		complete_pio(vcpu);
		return 1;
	}
	return 0;
}