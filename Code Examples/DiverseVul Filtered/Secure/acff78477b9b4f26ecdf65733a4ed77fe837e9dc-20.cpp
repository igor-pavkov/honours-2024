static void nested_cache_shadow_vmcs12(struct kvm_vcpu *vcpu,
				       struct vmcs12 *vmcs12)
{
	struct vmcs12 *shadow;
	struct page *page;

	if (!nested_cpu_has_shadow_vmcs(vmcs12) ||
	    vmcs12->vmcs_link_pointer == -1ull)
		return;

	shadow = get_shadow_vmcs12(vcpu);
	page = kvm_vcpu_gpa_to_page(vcpu, vmcs12->vmcs_link_pointer);

	memcpy(shadow, kmap(page), VMCS12_SIZE);

	kunmap(page);
	kvm_release_page_clean(page);
}