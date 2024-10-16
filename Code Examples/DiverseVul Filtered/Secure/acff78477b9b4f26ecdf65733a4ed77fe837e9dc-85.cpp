static int handle_vmptrld(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	gpa_t vmptr;

	if (!nested_vmx_check_permission(vcpu))
		return 1;

	if (nested_vmx_get_vmptr(vcpu, &vmptr))
		return 1;

	if (!PAGE_ALIGNED(vmptr) || (vmptr >> cpuid_maxphyaddr(vcpu)))
		return nested_vmx_failValid(vcpu,
			VMXERR_VMPTRLD_INVALID_ADDRESS);

	if (vmptr == vmx->nested.vmxon_ptr)
		return nested_vmx_failValid(vcpu,
			VMXERR_VMPTRLD_VMXON_POINTER);

	/* Forbid normal VMPTRLD if Enlightened version was used */
	if (vmx->nested.hv_evmcs)
		return 1;

	if (vmx->nested.current_vmptr != vmptr) {
		struct vmcs12 *new_vmcs12;
		struct page *page;

		page = kvm_vcpu_gpa_to_page(vcpu, vmptr);
		if (is_error_page(page)) {
			/*
			 * Reads from an unbacked page return all 1s,
			 * which means that the 32 bits located at the
			 * given physical address won't match the required
			 * VMCS12_REVISION identifier.
			 */
			return nested_vmx_failValid(vcpu,
				VMXERR_VMPTRLD_INCORRECT_VMCS_REVISION_ID);
		}
		new_vmcs12 = kmap(page);
		if (new_vmcs12->hdr.revision_id != VMCS12_REVISION ||
		    (new_vmcs12->hdr.shadow_vmcs &&
		     !nested_cpu_has_vmx_shadow_vmcs(vcpu))) {
			kunmap(page);
			kvm_release_page_clean(page);
			return nested_vmx_failValid(vcpu,
				VMXERR_VMPTRLD_INCORRECT_VMCS_REVISION_ID);
		}

		nested_release_vmcs12(vcpu);

		/*
		 * Load VMCS12 from guest memory since it is not already
		 * cached.
		 */
		memcpy(vmx->nested.cached_vmcs12, new_vmcs12, VMCS12_SIZE);
		kunmap(page);
		kvm_release_page_clean(page);

		set_current_vmptr(vmx, vmptr);
	}

	return nested_vmx_succeed(vcpu);
}