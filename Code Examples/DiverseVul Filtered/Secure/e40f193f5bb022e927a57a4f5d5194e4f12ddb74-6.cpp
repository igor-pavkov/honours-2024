void kvm_release_pfn_clean(pfn_t pfn)
{
	if (!is_error_noslot_pfn(pfn) && !kvm_is_mmio_pfn(pfn))
		put_page(pfn_to_page(pfn));
}