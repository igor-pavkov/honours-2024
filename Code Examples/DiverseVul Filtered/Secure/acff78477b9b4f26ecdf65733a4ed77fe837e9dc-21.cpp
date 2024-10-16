static int nested_check_vm_entry_controls(struct kvm_vcpu *vcpu,
					  struct vmcs12 *vmcs12)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!vmx_control_verify(vmcs12->vm_entry_controls,
				vmx->nested.msrs.entry_ctls_low,
				vmx->nested.msrs.entry_ctls_high))
		return -EINVAL;

	/*
	 * From the Intel SDM, volume 3:
	 * Fields relevant to VM-entry event injection must be set properly.
	 * These fields are the VM-entry interruption-information field, the
	 * VM-entry exception error code, and the VM-entry instruction length.
	 */
	if (vmcs12->vm_entry_intr_info_field & INTR_INFO_VALID_MASK) {
		u32 intr_info = vmcs12->vm_entry_intr_info_field;
		u8 vector = intr_info & INTR_INFO_VECTOR_MASK;
		u32 intr_type = intr_info & INTR_INFO_INTR_TYPE_MASK;
		bool has_error_code = intr_info & INTR_INFO_DELIVER_CODE_MASK;
		bool should_have_error_code;
		bool urg = nested_cpu_has2(vmcs12,
					   SECONDARY_EXEC_UNRESTRICTED_GUEST);
		bool prot_mode = !urg || vmcs12->guest_cr0 & X86_CR0_PE;

		/* VM-entry interruption-info field: interruption type */
		if (intr_type == INTR_TYPE_RESERVED ||
		    (intr_type == INTR_TYPE_OTHER_EVENT &&
		     !nested_cpu_supports_monitor_trap_flag(vcpu)))
			return -EINVAL;

		/* VM-entry interruption-info field: vector */
		if ((intr_type == INTR_TYPE_NMI_INTR && vector != NMI_VECTOR) ||
		    (intr_type == INTR_TYPE_HARD_EXCEPTION && vector > 31) ||
		    (intr_type == INTR_TYPE_OTHER_EVENT && vector != 0))
			return -EINVAL;

		/* VM-entry interruption-info field: deliver error code */
		should_have_error_code =
			intr_type == INTR_TYPE_HARD_EXCEPTION && prot_mode &&
			x86_exception_has_error_code(vector);
		if (has_error_code != should_have_error_code)
			return -EINVAL;

		/* VM-entry exception error code */
		if (has_error_code &&
		    vmcs12->vm_entry_exception_error_code & GENMASK(31, 15))
			return -EINVAL;

		/* VM-entry interruption-info field: reserved bits */
		if (intr_info & INTR_INFO_RESVD_BITS_MASK)
			return -EINVAL;

		/* VM-entry instruction length */
		switch (intr_type) {
		case INTR_TYPE_SOFT_EXCEPTION:
		case INTR_TYPE_SOFT_INTR:
		case INTR_TYPE_PRIV_SW_EXCEPTION:
			if ((vmcs12->vm_entry_instruction_len > 15) ||
			    (vmcs12->vm_entry_instruction_len == 0 &&
			     !nested_cpu_has_zero_length_injection(vcpu)))
				return -EINVAL;
		}
	}

	if (nested_vmx_check_entry_msr_switch_controls(vcpu, vmcs12))
		return -EINVAL;

	return 0;
}