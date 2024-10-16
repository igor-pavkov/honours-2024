static int handle_vmclear(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 zero = 0;
	gpa_t vmptr;

	if (!nested_vmx_check_permission(vcpu))
		return 1;

	if (nested_vmx_get_vmptr(vcpu, &vmptr))
		return 1;

	if (!PAGE_ALIGNED(vmptr) || (vmptr >> cpuid_maxphyaddr(vcpu)))
		return nested_vmx_failValid(vcpu,
			VMXERR_VMCLEAR_INVALID_ADDRESS);

	if (vmptr == vmx->nested.vmxon_ptr)
		return nested_vmx_failValid(vcpu,
			VMXERR_VMCLEAR_VMXON_POINTER);

	if (vmx->nested.hv_evmcs_page) {
		if (vmptr == vmx->nested.hv_evmcs_vmptr)
			nested_release_evmcs(vcpu);
	} else {
		if (vmptr == vmx->nested.current_vmptr)
			nested_release_vmcs12(vcpu);

		kvm_vcpu_write_guest(vcpu,
				     vmptr + offsetof(struct vmcs12,
						      launch_state),
				     &zero, sizeof(zero));
	}

	return nested_vmx_succeed(vcpu);
}