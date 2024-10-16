static int kvm_fill_hv_flush_list_func(struct hv_guest_mapping_flush_list *flush,
		void *data)
{
	struct kvm_tlb_range *range = data;

	return hyperv_fill_flush_guest_mapping_list(flush, range->start_gfn,
			range->pages);
}