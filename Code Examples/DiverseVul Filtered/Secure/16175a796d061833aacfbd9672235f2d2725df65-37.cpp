static int handle_exception(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 intr_info, ex_no, error_code;
	unsigned long cr2, rip, dr6;
	u32 vect_info;
	enum emulation_result er;

	vect_info = vmx->idt_vectoring_info;
	intr_info = vmcs_read32(VM_EXIT_INTR_INFO);

	if ((vect_info & VECTORING_INFO_VALID_MASK) &&
						!is_page_fault(intr_info))
		printk(KERN_ERR "%s: unexpected, vectoring info 0x%x "
		       "intr info 0x%x\n", __func__, vect_info, intr_info);

	if (!irqchip_in_kernel(vcpu->kvm) && is_external_interrupt(vect_info)) {
		int irq = vect_info & VECTORING_INFO_VECTOR_MASK;
		set_bit(irq, vcpu->arch.irq_pending);
		set_bit(irq / BITS_PER_LONG, &vcpu->arch.irq_summary);
	}

	if ((intr_info & INTR_INFO_INTR_TYPE_MASK) == INTR_TYPE_NMI_INTR)
		return 1;  /* already handled by vmx_vcpu_run() */

	if (is_no_device(intr_info)) {
		vmx_fpu_activate(vcpu);
		return 1;
	}

	if (is_invalid_opcode(intr_info)) {
		er = emulate_instruction(vcpu, kvm_run, 0, 0, EMULTYPE_TRAP_UD);
		if (er != EMULATE_DONE)
			kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

	error_code = 0;
	rip = kvm_rip_read(vcpu);
	if (intr_info & INTR_INFO_DELIVER_CODE_MASK)
		error_code = vmcs_read32(VM_EXIT_INTR_ERROR_CODE);
	if (is_page_fault(intr_info)) {
		/* EPT won't cause page fault directly */
		if (vm_need_ept())
			BUG();
		cr2 = vmcs_readl(EXIT_QUALIFICATION);
		KVMTRACE_3D(PAGE_FAULT, vcpu, error_code, (u32)cr2,
			    (u32)((u64)cr2 >> 32), handler);
		if (vcpu->arch.interrupt.pending || vcpu->arch.exception.pending)
			kvm_mmu_unprotect_page_virt(vcpu, cr2);
		return kvm_mmu_page_fault(vcpu, cr2, error_code);
	}

	if (vcpu->arch.rmode.active &&
	    handle_rmode_exception(vcpu, intr_info & INTR_INFO_VECTOR_MASK,
								error_code)) {
		if (vcpu->arch.halt_request) {
			vcpu->arch.halt_request = 0;
			return kvm_emulate_halt(vcpu);
		}
		return 1;
	}

	ex_no = intr_info & INTR_INFO_VECTOR_MASK;
	switch (ex_no) {
	case DB_VECTOR:
		dr6 = vmcs_readl(EXIT_QUALIFICATION);
		if (!(vcpu->guest_debug &
		      (KVM_GUESTDBG_SINGLESTEP | KVM_GUESTDBG_USE_HW_BP))) {
			vcpu->arch.dr6 = dr6 | DR6_FIXED_1;
			kvm_queue_exception(vcpu, DB_VECTOR);
			return 1;
		}
		kvm_run->debug.arch.dr6 = dr6 | DR6_FIXED_1;
		kvm_run->debug.arch.dr7 = vmcs_readl(GUEST_DR7);
		/* fall through */
	case BP_VECTOR:
		kvm_run->exit_reason = KVM_EXIT_DEBUG;
		kvm_run->debug.arch.pc = vmcs_readl(GUEST_CS_BASE) + rip;
		kvm_run->debug.arch.exception = ex_no;
		break;
	default:
		kvm_run->exit_reason = KVM_EXIT_EXCEPTION;
		kvm_run->ex.exception = ex_no;
		kvm_run->ex.error_code = error_code;
		break;
	}
	return 0;
}