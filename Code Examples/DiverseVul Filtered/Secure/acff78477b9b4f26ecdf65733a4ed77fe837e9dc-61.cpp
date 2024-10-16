static int nested_vmx_check_vmcs_link_ptr(struct kvm_vcpu *vcpu,
					  struct vmcs12 *vmcs12)
{
	int r;
	struct page *page;
	struct vmcs12 *shadow;

	if (vmcs12->vmcs_link_pointer == -1ull)
		return 0;

	if (!page_address_valid(vcpu, vmcs12->vmcs_link_pointer))
		return -EINVAL;

	page = kvm_vcpu_gpa_to_page(vcpu, vmcs12->vmcs_link_pointer);
	if (is_error_page(page))
		return -EINVAL;

	r = 0;
	shadow = kmap(page);
	if (shadow->hdr.revision_id != VMCS12_REVISION ||
	    shadow->hdr.shadow_vmcs != nested_cpu_has_shadow_vmcs(vmcs12))
		r = -EINVAL;
	kunmap(page);
	kvm_release_page_clean(page);
	return r;
}