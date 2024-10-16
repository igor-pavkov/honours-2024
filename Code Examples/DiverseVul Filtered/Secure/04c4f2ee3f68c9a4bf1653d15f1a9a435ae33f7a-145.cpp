static void vmx_get_exit_info(struct kvm_vcpu *vcpu, u64 *info1, u64 *info2,
			      u32 *intr_info, u32 *error_code)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	*info1 = vmx_get_exit_qual(vcpu);
	if (!(vmx->exit_reason.failed_vmentry)) {
		*info2 = vmx->idt_vectoring_info;
		*intr_info = vmx_get_intr_info(vcpu);
		if (is_exception_with_error_code(*intr_info))
			*error_code = vmcs_read32(VM_EXIT_INTR_ERROR_CODE);
		else
			*error_code = 0;
	} else {
		*info2 = 0;
		*intr_info = 0;
		*error_code = 0;
	}
}