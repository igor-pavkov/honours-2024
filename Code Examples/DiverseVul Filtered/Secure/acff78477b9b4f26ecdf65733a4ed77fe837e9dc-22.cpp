static int handle_invvpid(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 vmx_instruction_info;
	unsigned long type, types;
	gva_t gva;
	struct x86_exception e;
	struct {
		u64 vpid;
		u64 gla;
	} operand;
	u16 vpid02;

	if (!(vmx->nested.msrs.secondary_ctls_high &
	      SECONDARY_EXEC_ENABLE_VPID) ||
			!(vmx->nested.msrs.vpid_caps & VMX_VPID_INVVPID_BIT)) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

	if (!nested_vmx_check_permission(vcpu))
		return 1;

	vmx_instruction_info = vmcs_read32(VMX_INSTRUCTION_INFO);
	type = kvm_register_readl(vcpu, (vmx_instruction_info >> 28) & 0xf);

	types = (vmx->nested.msrs.vpid_caps &
			VMX_VPID_EXTENT_SUPPORTED_MASK) >> 8;

	if (type >= 32 || !(types & (1 << type)))
		return nested_vmx_failValid(vcpu,
			VMXERR_INVALID_OPERAND_TO_INVEPT_INVVPID);

	/* according to the intel vmx instruction reference, the memory
	 * operand is read even if it isn't needed (e.g., for type==global)
	 */
	if (get_vmx_mem_address(vcpu, vmcs_readl(EXIT_QUALIFICATION),
			vmx_instruction_info, false, &gva))
		return 1;
	if (kvm_read_guest_virt(vcpu, gva, &operand, sizeof(operand), &e)) {
		kvm_inject_page_fault(vcpu, &e);
		return 1;
	}
	if (operand.vpid >> 16)
		return nested_vmx_failValid(vcpu,
			VMXERR_INVALID_OPERAND_TO_INVEPT_INVVPID);

	vpid02 = nested_get_vpid02(vcpu);
	switch (type) {
	case VMX_VPID_EXTENT_INDIVIDUAL_ADDR:
		if (!operand.vpid ||
		    is_noncanonical_address(operand.gla, vcpu))
			return nested_vmx_failValid(vcpu,
				VMXERR_INVALID_OPERAND_TO_INVEPT_INVVPID);
		if (cpu_has_vmx_invvpid_individual_addr()) {
			__invvpid(VMX_VPID_EXTENT_INDIVIDUAL_ADDR,
				vpid02, operand.gla);
		} else
			__vmx_flush_tlb(vcpu, vpid02, false);
		break;
	case VMX_VPID_EXTENT_SINGLE_CONTEXT:
	case VMX_VPID_EXTENT_SINGLE_NON_GLOBAL:
		if (!operand.vpid)
			return nested_vmx_failValid(vcpu,
				VMXERR_INVALID_OPERAND_TO_INVEPT_INVVPID);
		__vmx_flush_tlb(vcpu, vpid02, false);
		break;
	case VMX_VPID_EXTENT_ALL_CONTEXT:
		__vmx_flush_tlb(vcpu, vpid02, false);
		break;
	default:
		WARN_ON_ONCE(1);
		return kvm_skip_emulated_instruction(vcpu);
	}

	return nested_vmx_succeed(vcpu);
}