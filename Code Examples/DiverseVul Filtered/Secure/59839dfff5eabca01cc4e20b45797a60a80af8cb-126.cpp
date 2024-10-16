int complete_pio(struct kvm_vcpu *vcpu)
{
	struct kvm_pio_request *io = &vcpu->arch.pio;
	long delta;
	int r;
	unsigned long val;

	if (!io->string) {
		if (io->in) {
			val = kvm_register_read(vcpu, VCPU_REGS_RAX);
			memcpy(&val, vcpu->arch.pio_data, io->size);
			kvm_register_write(vcpu, VCPU_REGS_RAX, val);
		}
	} else {
		if (io->in) {
			r = pio_copy_data(vcpu);
			if (r)
				return r;
		}

		delta = 1;
		if (io->rep) {
			delta *= io->cur_count;
			/*
			 * The size of the register should really depend on
			 * current address size.
			 */
			val = kvm_register_read(vcpu, VCPU_REGS_RCX);
			val -= delta;
			kvm_register_write(vcpu, VCPU_REGS_RCX, val);
		}
		if (io->down)
			delta = -delta;
		delta *= io->size;
		if (io->in) {
			val = kvm_register_read(vcpu, VCPU_REGS_RDI);
			val += delta;
			kvm_register_write(vcpu, VCPU_REGS_RDI, val);
		} else {
			val = kvm_register_read(vcpu, VCPU_REGS_RSI);
			val += delta;
			kvm_register_write(vcpu, VCPU_REGS_RSI, val);
		}
	}

	io->count -= io->cur_count;
	io->cur_count = 0;

	return 0;
}