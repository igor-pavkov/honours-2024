static int handle_vmptrld(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	gva_t gva;
	gpa_t vmptr;
	struct x86_exception e;
	u32 exec_control;

	if (!nested_vmx_check_permission(vcpu))
		return 1;

	if (get_vmx_mem_address(vcpu, vmcs_readl(EXIT_QUALIFICATION),
			vmcs_read32(VMX_INSTRUCTION_INFO), &gva))
		return 1;

	if (kvm_read_guest_virt(&vcpu->arch.emulate_ctxt, gva, &vmptr,
				sizeof(vmptr), &e)) {
		kvm_inject_page_fault(vcpu, &e);
		return 1;
	}

	if (!IS_ALIGNED(vmptr, PAGE_SIZE)) {
		nested_vmx_failValid(vcpu, VMXERR_VMPTRLD_INVALID_ADDRESS);
		skip_emulated_instruction(vcpu);
		return 1;
	}

	if (vmx->nested.current_vmptr != vmptr) {
		struct vmcs12 *new_vmcs12;
		struct page *page;
		page = nested_get_page(vcpu, vmptr);
		if (page == NULL) {
			nested_vmx_failInvalid(vcpu);
			skip_emulated_instruction(vcpu);
			return 1;
		}
		new_vmcs12 = kmap(page);
		if (new_vmcs12->revision_id != VMCS12_REVISION) {
			kunmap(page);
			nested_release_page_clean(page);
			nested_vmx_failValid(vcpu,
				VMXERR_VMPTRLD_INCORRECT_VMCS_REVISION_ID);
			skip_emulated_instruction(vcpu);
			return 1;
		}
		if (vmx->nested.current_vmptr != -1ull)
			nested_release_vmcs12(vmx);

		vmx->nested.current_vmptr = vmptr;
		vmx->nested.current_vmcs12 = new_vmcs12;
		vmx->nested.current_vmcs12_page = page;
		if (enable_shadow_vmcs) {
			exec_control = vmcs_read32(SECONDARY_VM_EXEC_CONTROL);
			exec_control |= SECONDARY_EXEC_SHADOW_VMCS;
			vmcs_write32(SECONDARY_VM_EXEC_CONTROL, exec_control);
			vmcs_write64(VMCS_LINK_POINTER,
				     __pa(vmx->nested.current_shadow_vmcs));
			vmx->nested.sync_shadow_vmcs = true;
		}
	}

	nested_vmx_succeed(vcpu);
	skip_emulated_instruction(vcpu);
	return 1;
}