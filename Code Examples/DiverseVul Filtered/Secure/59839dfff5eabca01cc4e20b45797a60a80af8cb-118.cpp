int kvm_task_switch(struct kvm_vcpu *vcpu, u16 tss_selector, int reason)
{
	struct kvm_segment tr_seg;
	struct desc_struct cseg_desc;
	struct desc_struct nseg_desc;
	int ret = 0;
	u32 old_tss_base = get_segment_base(vcpu, VCPU_SREG_TR);
	u16 old_tss_sel = get_segment_selector(vcpu, VCPU_SREG_TR);

	old_tss_base = vcpu->arch.mmu.gva_to_gpa(vcpu, old_tss_base);

	/* FIXME: Handle errors. Failure to read either TSS or their
	 * descriptors should generate a pagefault.
	 */
	if (load_guest_segment_descriptor(vcpu, tss_selector, &nseg_desc))
		goto out;

	if (load_guest_segment_descriptor(vcpu, old_tss_sel, &cseg_desc))
		goto out;

	if (reason != TASK_SWITCH_IRET) {
		int cpl;

		cpl = kvm_x86_ops->get_cpl(vcpu);
		if ((tss_selector & 3) > nseg_desc.dpl || cpl > nseg_desc.dpl) {
			kvm_queue_exception_e(vcpu, GP_VECTOR, 0);
			return 1;
		}
	}

	if (!nseg_desc.p || (nseg_desc.limit0 | nseg_desc.limit << 16) < 0x67) {
		kvm_queue_exception_e(vcpu, TS_VECTOR, tss_selector & 0xfffc);
		return 1;
	}

	if (reason == TASK_SWITCH_IRET || reason == TASK_SWITCH_JMP) {
		cseg_desc.type &= ~(1 << 1); //clear the B flag
		save_guest_segment_descriptor(vcpu, old_tss_sel, &cseg_desc);
	}

	if (reason == TASK_SWITCH_IRET) {
		u32 eflags = kvm_x86_ops->get_rflags(vcpu);
		kvm_x86_ops->set_rflags(vcpu, eflags & ~X86_EFLAGS_NT);
	}

	/* set back link to prev task only if NT bit is set in eflags
	   note that old_tss_sel is not used afetr this point */
	if (reason != TASK_SWITCH_CALL && reason != TASK_SWITCH_GATE)
		old_tss_sel = 0xffff;

	/* set back link to prev task only if NT bit is set in eflags
	   note that old_tss_sel is not used afetr this point */
	if (reason != TASK_SWITCH_CALL && reason != TASK_SWITCH_GATE)
		old_tss_sel = 0xffff;

	if (nseg_desc.type & 8)
		ret = kvm_task_switch_32(vcpu, tss_selector, old_tss_sel,
					 old_tss_base, &nseg_desc);
	else
		ret = kvm_task_switch_16(vcpu, tss_selector, old_tss_sel,
					 old_tss_base, &nseg_desc);

	if (reason == TASK_SWITCH_CALL || reason == TASK_SWITCH_GATE) {
		u32 eflags = kvm_x86_ops->get_rflags(vcpu);
		kvm_x86_ops->set_rflags(vcpu, eflags | X86_EFLAGS_NT);
	}

	if (reason != TASK_SWITCH_IRET) {
		nseg_desc.type |= (1 << 1);
		save_guest_segment_descriptor(vcpu, tss_selector,
					      &nseg_desc);
	}

	kvm_x86_ops->set_cr0(vcpu, vcpu->arch.cr0 | X86_CR0_TS);
	seg_desct_to_kvm_desct(&nseg_desc, tss_selector, &tr_seg);
	tr_seg.type = 11;
	kvm_set_segment(vcpu, &tr_seg, VCPU_SREG_TR);
out:
	return ret;
}