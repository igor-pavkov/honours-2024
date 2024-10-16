static int nested_vmx_handle_enlightened_vmptrld(struct kvm_vcpu *vcpu,
						 bool from_launch)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct hv_vp_assist_page assist_page;

	if (likely(!vmx->nested.enlightened_vmcs_enabled))
		return 1;

	if (unlikely(!kvm_hv_get_assist_page(vcpu, &assist_page)))
		return 1;

	if (unlikely(!assist_page.enlighten_vmentry))
		return 1;

	if (unlikely(assist_page.current_nested_vmcs !=
		     vmx->nested.hv_evmcs_vmptr)) {

		if (!vmx->nested.hv_evmcs)
			vmx->nested.current_vmptr = -1ull;

		nested_release_evmcs(vcpu);

		vmx->nested.hv_evmcs_page = kvm_vcpu_gpa_to_page(
			vcpu, assist_page.current_nested_vmcs);

		if (unlikely(is_error_page(vmx->nested.hv_evmcs_page)))
			return 0;

		vmx->nested.hv_evmcs = kmap(vmx->nested.hv_evmcs_page);

		/*
		 * Currently, KVM only supports eVMCS version 1
		 * (== KVM_EVMCS_VERSION) and thus we expect guest to set this
		 * value to first u32 field of eVMCS which should specify eVMCS
		 * VersionNumber.
		 *
		 * Guest should be aware of supported eVMCS versions by host by
		 * examining CPUID.0x4000000A.EAX[0:15]. Host userspace VMM is
		 * expected to set this CPUID leaf according to the value
		 * returned in vmcs_version from nested_enable_evmcs().
		 *
		 * However, it turns out that Microsoft Hyper-V fails to comply
		 * to their own invented interface: When Hyper-V use eVMCS, it
		 * just sets first u32 field of eVMCS to revision_id specified
		 * in MSR_IA32_VMX_BASIC. Instead of used eVMCS version number
		 * which is one of the supported versions specified in
		 * CPUID.0x4000000A.EAX[0:15].
		 *
		 * To overcome Hyper-V bug, we accept here either a supported
		 * eVMCS version or VMCS12 revision_id as valid values for first
		 * u32 field of eVMCS.
		 */
		if ((vmx->nested.hv_evmcs->revision_id != KVM_EVMCS_VERSION) &&
		    (vmx->nested.hv_evmcs->revision_id != VMCS12_REVISION)) {
			nested_release_evmcs(vcpu);
			return 0;
		}

		vmx->nested.dirty_vmcs12 = true;
		/*
		 * As we keep L2 state for one guest only 'hv_clean_fields' mask
		 * can't be used when we switch between them. Reset it here for
		 * simplicity.
		 */
		vmx->nested.hv_evmcs->hv_clean_fields &=
			~HV_VMX_ENLIGHTENED_CLEAN_FIELD_ALL;
		vmx->nested.hv_evmcs_vmptr = assist_page.current_nested_vmcs;

		/*
		 * Unlike normal vmcs12, enlightened vmcs12 is not fully
		 * reloaded from guest's memory (read only fields, fields not
		 * present in struct hv_enlightened_vmcs, ...). Make sure there
		 * are no leftovers.
		 */
		if (from_launch) {
			struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
			memset(vmcs12, 0, sizeof(*vmcs12));
			vmcs12->hdr.revision_id = VMCS12_REVISION;
		}

	}
	return 1;
}