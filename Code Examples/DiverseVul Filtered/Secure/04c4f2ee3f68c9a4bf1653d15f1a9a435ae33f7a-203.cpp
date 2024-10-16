static int vmx_get_msr_feature(struct kvm_msr_entry *msr)
{
	switch (msr->index) {
	case MSR_IA32_VMX_BASIC ... MSR_IA32_VMX_VMFUNC:
		if (!nested)
			return 1;
		return vmx_get_vmx_msr(&vmcs_config.nested, msr->index, &msr->data);
	case MSR_IA32_PERF_CAPABILITIES:
		msr->data = vmx_get_perf_capabilities();
		return 0;
	default:
		return KVM_MSR_RET_INVALID;
	}
}