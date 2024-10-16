static int handle_vmread(struct kvm_vcpu *vcpu)
{
	unsigned long field;
	u64 field_value;
	unsigned long exit_qualification = vmcs_readl(EXIT_QUALIFICATION);
	u32 vmx_instruction_info = vmcs_read32(VMX_INSTRUCTION_INFO);
	gva_t gva = 0;
	struct vmcs12 *vmcs12;

	if (!nested_vmx_check_permission(vcpu))
		return 1;

	if (to_vmx(vcpu)->nested.current_vmptr == -1ull)
		return nested_vmx_failInvalid(vcpu);

	if (!is_guest_mode(vcpu))
		vmcs12 = get_vmcs12(vcpu);
	else {
		/*
		 * When vmcs->vmcs_link_pointer is -1ull, any VMREAD
		 * to shadowed-field sets the ALU flags for VMfailInvalid.
		 */
		if (get_vmcs12(vcpu)->vmcs_link_pointer == -1ull)
			return nested_vmx_failInvalid(vcpu);
		vmcs12 = get_shadow_vmcs12(vcpu);
	}

	/* Decode instruction info and find the field to read */
	field = kvm_register_readl(vcpu, (((vmx_instruction_info) >> 28) & 0xf));
	/* Read the field, zero-extended to a u64 field_value */
	if (vmcs12_read_any(vmcs12, field, &field_value) < 0)
		return nested_vmx_failValid(vcpu,
			VMXERR_UNSUPPORTED_VMCS_COMPONENT);

	/*
	 * Now copy part of this value to register or memory, as requested.
	 * Note that the number of bits actually copied is 32 or 64 depending
	 * on the guest's mode (32 or 64 bit), not on the given field's length.
	 */
	if (vmx_instruction_info & (1u << 10)) {
		kvm_register_writel(vcpu, (((vmx_instruction_info) >> 3) & 0xf),
			field_value);
	} else {
		if (get_vmx_mem_address(vcpu, exit_qualification,
				vmx_instruction_info, true, &gva))
			return 1;
		/* _system ok, nested_vmx_check_permission has verified cpl=0 */
		kvm_write_guest_virt_system(vcpu, gva, &field_value,
					    (is_long_mode(vcpu) ? 8 : 4), NULL);
	}

	return nested_vmx_succeed(vcpu);
}