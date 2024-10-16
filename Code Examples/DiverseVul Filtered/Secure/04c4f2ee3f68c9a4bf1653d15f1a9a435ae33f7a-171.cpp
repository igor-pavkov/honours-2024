static bool vmx_has_emulated_msr(struct kvm *kvm, u32 index)
{
	switch (index) {
	case MSR_IA32_SMBASE:
		/*
		 * We cannot do SMM unless we can run the guest in big
		 * real mode.
		 */
		return enable_unrestricted_guest || emulate_invalid_guest_state;
	case MSR_IA32_VMX_BASIC ... MSR_IA32_VMX_VMFUNC:
		return nested;
	case MSR_AMD64_VIRT_SPEC_CTRL:
		/* This is AMD only.  */
		return false;
	default:
		return true;
	}
}