void nested_sync_from_vmcs12(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	/*
	 * hv_evmcs may end up being not mapped after migration (when
	 * L2 was running), map it here to make sure vmcs12 changes are
	 * properly reflected.
	 */
	if (vmx->nested.enlightened_vmcs_enabled && !vmx->nested.hv_evmcs)
		nested_vmx_handle_enlightened_vmptrld(vcpu, false);

	if (vmx->nested.hv_evmcs) {
		copy_vmcs12_to_enlightened(vmx);
		/* All fields are clean */
		vmx->nested.hv_evmcs->hv_clean_fields |=
			HV_VMX_ENLIGHTENED_CLEAN_FIELD_ALL;
	} else {
		copy_vmcs12_to_shadow(vmx);
	}

	vmx->nested.need_vmcs12_sync = false;
}