static void vmx_complete_interrupts(struct vcpu_vmx *vmx)
{
	u32 exit_intr_info;
	u32 idt_vectoring_info;
	bool unblock_nmi;
	u8 vector;
	int type;
	bool idtv_info_valid;
	u32 error;

	exit_intr_info = vmcs_read32(VM_EXIT_INTR_INFO);
	if (cpu_has_virtual_nmis()) {
		unblock_nmi = (exit_intr_info & INTR_INFO_UNBLOCK_NMI) != 0;
		vector = exit_intr_info & INTR_INFO_VECTOR_MASK;
		/*
		 * SDM 3: 25.7.1.2
		 * Re-set bit "block by NMI" before VM entry if vmexit caused by
		 * a guest IRET fault.
		 */
		if (unblock_nmi && vector != DF_VECTOR)
			vmcs_set_bits(GUEST_INTERRUPTIBILITY_INFO,
				      GUEST_INTR_STATE_NMI);
	} else if (unlikely(vmx->soft_vnmi_blocked))
		vmx->vnmi_blocked_time +=
			ktime_to_ns(ktime_sub(ktime_get(), vmx->entry_time));

	idt_vectoring_info = vmx->idt_vectoring_info;
	idtv_info_valid = idt_vectoring_info & VECTORING_INFO_VALID_MASK;
	vector = idt_vectoring_info & VECTORING_INFO_VECTOR_MASK;
	type = idt_vectoring_info & VECTORING_INFO_TYPE_MASK;
	if (vmx->vcpu.arch.nmi_injected) {
		/*
		 * SDM 3: 25.7.1.2
		 * Clear bit "block by NMI" before VM entry if a NMI delivery
		 * faulted.
		 */
		if (idtv_info_valid && type == INTR_TYPE_NMI_INTR)
			vmcs_clear_bits(GUEST_INTERRUPTIBILITY_INFO,
					GUEST_INTR_STATE_NMI);
		else
			vmx->vcpu.arch.nmi_injected = false;
	}
	kvm_clear_exception_queue(&vmx->vcpu);
	if (idtv_info_valid && (type == INTR_TYPE_HARD_EXCEPTION ||
				type == INTR_TYPE_SOFT_EXCEPTION)) {
		if (idt_vectoring_info & VECTORING_INFO_DELIVER_CODE_MASK) {
			error = vmcs_read32(IDT_VECTORING_ERROR_CODE);
			kvm_queue_exception_e(&vmx->vcpu, vector, error);
		} else
			kvm_queue_exception(&vmx->vcpu, vector);
		vmx->idt_vectoring_info = 0;
	}
	kvm_clear_interrupt_queue(&vmx->vcpu);
	if (idtv_info_valid && type == INTR_TYPE_EXT_INTR) {
		kvm_queue_interrupt(&vmx->vcpu, vector);
		vmx->idt_vectoring_info = 0;
	}
}