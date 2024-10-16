static int handle_invpcid(struct kvm_vcpu *vcpu)
{
	u32 vmx_instruction_info;
	unsigned long type;
	gva_t gva;
	struct {
		u64 pcid;
		u64 gla;
	} operand;

	if (!guest_cpuid_has(vcpu, X86_FEATURE_INVPCID)) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

	vmx_instruction_info = vmcs_read32(VMX_INSTRUCTION_INFO);
	type = kvm_register_readl(vcpu, (vmx_instruction_info >> 28) & 0xf);

	if (type > 3) {
		kvm_inject_gp(vcpu, 0);
		return 1;
	}

	/* According to the Intel instruction reference, the memory operand
	 * is read even if it isn't needed (e.g., for type==all)
	 */
	if (get_vmx_mem_address(vcpu, vmx_get_exit_qual(vcpu),
				vmx_instruction_info, false,
				sizeof(operand), &gva))
		return 1;

	return kvm_handle_invpcid(vcpu, type, gva);
}