static int load_state_from_tss32(struct kvm_vcpu *vcpu,
				  struct tss_segment_32 *tss)
{
	kvm_set_cr3(vcpu, tss->cr3);

	kvm_rip_write(vcpu, tss->eip);
	kvm_x86_ops->set_rflags(vcpu, tss->eflags | 2);

	kvm_register_write(vcpu, VCPU_REGS_RAX, tss->eax);
	kvm_register_write(vcpu, VCPU_REGS_RCX, tss->ecx);
	kvm_register_write(vcpu, VCPU_REGS_RDX, tss->edx);
	kvm_register_write(vcpu, VCPU_REGS_RBX, tss->ebx);
	kvm_register_write(vcpu, VCPU_REGS_RSP, tss->esp);
	kvm_register_write(vcpu, VCPU_REGS_RBP, tss->ebp);
	kvm_register_write(vcpu, VCPU_REGS_RSI, tss->esi);
	kvm_register_write(vcpu, VCPU_REGS_RDI, tss->edi);

	if (kvm_load_segment_descriptor(vcpu, tss->ldt_selector, 0, VCPU_SREG_LDTR))
		return 1;

	if (kvm_load_segment_descriptor(vcpu, tss->es, 1, VCPU_SREG_ES))
		return 1;

	if (kvm_load_segment_descriptor(vcpu, tss->cs, 9, VCPU_SREG_CS))
		return 1;

	if (kvm_load_segment_descriptor(vcpu, tss->ss, 1, VCPU_SREG_SS))
		return 1;

	if (kvm_load_segment_descriptor(vcpu, tss->ds, 1, VCPU_SREG_DS))
		return 1;

	if (kvm_load_segment_descriptor(vcpu, tss->fs, 1, VCPU_SREG_FS))
		return 1;

	if (kvm_load_segment_descriptor(vcpu, tss->gs, 1, VCPU_SREG_GS))
		return 1;
	return 0;
}