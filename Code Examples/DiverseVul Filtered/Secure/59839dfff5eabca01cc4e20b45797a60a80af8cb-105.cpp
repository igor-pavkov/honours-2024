static void save_state_to_tss16(struct kvm_vcpu *vcpu,
				struct tss_segment_16 *tss)
{
	tss->ip = kvm_rip_read(vcpu);
	tss->flag = kvm_x86_ops->get_rflags(vcpu);
	tss->ax = kvm_register_read(vcpu, VCPU_REGS_RAX);
	tss->cx = kvm_register_read(vcpu, VCPU_REGS_RCX);
	tss->dx = kvm_register_read(vcpu, VCPU_REGS_RDX);
	tss->bx = kvm_register_read(vcpu, VCPU_REGS_RBX);
	tss->sp = kvm_register_read(vcpu, VCPU_REGS_RSP);
	tss->bp = kvm_register_read(vcpu, VCPU_REGS_RBP);
	tss->si = kvm_register_read(vcpu, VCPU_REGS_RSI);
	tss->di = kvm_register_read(vcpu, VCPU_REGS_RDI);

	tss->es = get_segment_selector(vcpu, VCPU_SREG_ES);
	tss->cs = get_segment_selector(vcpu, VCPU_SREG_CS);
	tss->ss = get_segment_selector(vcpu, VCPU_SREG_SS);
	tss->ds = get_segment_selector(vcpu, VCPU_SREG_DS);
	tss->ldt = get_segment_selector(vcpu, VCPU_SREG_LDTR);
	tss->prev_task_link = get_segment_selector(vcpu, VCPU_SREG_TR);
}