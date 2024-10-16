static bool vmx_is_valid_cr4(struct kvm_vcpu *vcpu, unsigned long cr4)
{
	/*
	 * We operate under the default treatment of SMM, so VMX cannot be
	 * enabled under SMM.  Note, whether or not VMXE is allowed at all is
	 * handled by kvm_is_valid_cr4().
	 */
	if ((cr4 & X86_CR4_VMXE) && is_smm(vcpu))
		return false;

	if (to_vmx(vcpu)->nested.vmxon && !nested_cr4_valid(vcpu, cr4))
		return false;

	return true;
}