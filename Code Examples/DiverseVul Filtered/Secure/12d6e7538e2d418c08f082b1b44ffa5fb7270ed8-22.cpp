static struct page *kvm_pfn_to_page(pfn_t pfn)
{
	if (is_error_pfn(pfn))
		return KVM_ERR_PTR_BAD_PAGE;

	if (kvm_is_mmio_pfn(pfn)) {
		WARN_ON(1);
		return KVM_ERR_PTR_BAD_PAGE;
	}

	return pfn_to_page(pfn);
}