static int load_state_from_tss16(struct kvm_vcpu *vcpu,
				 struct tss_segment_16 *tss)
{
	kvm_rip_write(vcpu, tss->ip);
	kvm_x86_ops->set_rflags(vcpu, tss->flag | 2);
	kvm_register_write(vcpu, VCPU_REGS_RAX, tss->ax);
	kvm_register_write(vcpu, VCPU_REGS_RCX, tss->cx);
	kvm_register_write(vcpu, VCPU_REGS_RDX, tss->dx);
	kvm_register_write(vcpu, VCPU_REGS_RBX, tss->bx);
	kvm_register_write(vcpu, VCPU_REGS_RSP, tss->sp);
	kvm_register_write(vcpu, VCPU_REGS_RBP, tss->bp);
	kvm_register_write(vcpu, VCPU_REGS_RSI, tss->si);
	kvm_register_write(vcpu, VCPU_REGS_RDI, tss->di);

	if (kvm_load_segment_descriptor(vcpu, tss->ldt, 0, VCPU_SREG_LDTR))
		return 1;

	if (kvm_load_segment_descriptor(vcpu, tss->es, 1, VCPU_SREG_ES))
		return 1;

	if (kvm_load_segment_descriptor(vcpu, tss->cs, 9, VCPU_SREG_CS))
		return 1;

	if (kvm_load_segment_descriptor(vcpu, tss->ss, 1, VCPU_SREG_SS))
		return 1;

	if (kvm_load_segment_descriptor(vcpu, tss->ds, 1, VCPU_SREG_DS))
		return 1;
	return 0;
}