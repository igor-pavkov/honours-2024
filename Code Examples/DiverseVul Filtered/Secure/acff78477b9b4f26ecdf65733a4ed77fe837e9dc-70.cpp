static int handle_vmptrst(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qual = vmcs_readl(EXIT_QUALIFICATION);
	u32 instr_info = vmcs_read32(VMX_INSTRUCTION_INFO);
	gpa_t current_vmptr = to_vmx(vcpu)->nested.current_vmptr;
	struct x86_exception e;
	gva_t gva;

	if (!nested_vmx_check_permission(vcpu))
		return 1;

	if (unlikely(to_vmx(vcpu)->nested.hv_evmcs))
		return 1;

	if (get_vmx_mem_address(vcpu, exit_qual, instr_info, true, &gva))
		return 1;
	/* *_system ok, nested_vmx_check_permission has verified cpl=0 */
	if (kvm_write_guest_virt_system(vcpu, gva, (void *)&current_vmptr,
					sizeof(gpa_t), &e)) {
		kvm_inject_page_fault(vcpu, &e);
		return 1;
	}
	return nested_vmx_succeed(vcpu);
}