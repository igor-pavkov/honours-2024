static int handle_io(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	unsigned long exit_qualification;
	int size, in, string;
	unsigned port;

	++vcpu->stat.io_exits;
	exit_qualification = vmcs_readl(EXIT_QUALIFICATION);
	string = (exit_qualification & 16) != 0;

	if (string) {
		if (emulate_instruction(vcpu,
					kvm_run, 0, 0, 0) == EMULATE_DO_MMIO)
			return 0;
		return 1;
	}

	size = (exit_qualification & 7) + 1;
	in = (exit_qualification & 8) != 0;
	port = exit_qualification >> 16;

	skip_emulated_instruction(vcpu);
	return kvm_emulate_pio(vcpu, kvm_run, in, size, port);
}