static void vmx_queue_exception(struct kvm_vcpu *vcpu, unsigned nr,
				bool has_error_code, u32 error_code)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 intr_info = nr | INTR_INFO_VALID_MASK;

	if (has_error_code) {
		vmcs_write32(VM_ENTRY_EXCEPTION_ERROR_CODE, error_code);
		intr_info |= INTR_INFO_DELIVER_CODE_MASK;
	}

	if (vcpu->arch.rmode.active) {
		vmx->rmode.irq.pending = true;
		vmx->rmode.irq.vector = nr;
		vmx->rmode.irq.rip = kvm_rip_read(vcpu);
		if (nr == BP_VECTOR || nr == OF_VECTOR)
			vmx->rmode.irq.rip++;
		intr_info |= INTR_TYPE_SOFT_INTR;
		vmcs_write32(VM_ENTRY_INTR_INFO_FIELD, intr_info);
		vmcs_write32(VM_ENTRY_INSTRUCTION_LEN, 1);
		kvm_rip_write(vcpu, vmx->rmode.irq.rip - 1);
		return;
	}

	if (nr == BP_VECTOR || nr == OF_VECTOR) {
		vmcs_write32(VM_ENTRY_INSTRUCTION_LEN, 1);
		intr_info |= INTR_TYPE_SOFT_EXCEPTION;
	} else
		intr_info |= INTR_TYPE_HARD_EXCEPTION;

	vmcs_write32(VM_ENTRY_INTR_INFO_FIELD, intr_info);
}