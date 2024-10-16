static inline void nested_release_evmcs(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!vmx->nested.hv_evmcs)
		return;

	kunmap(vmx->nested.hv_evmcs_page);
	kvm_release_page_dirty(vmx->nested.hv_evmcs_page);
	vmx->nested.hv_evmcs_vmptr = -1ull;
	vmx->nested.hv_evmcs_page = NULL;
	vmx->nested.hv_evmcs = NULL;
}