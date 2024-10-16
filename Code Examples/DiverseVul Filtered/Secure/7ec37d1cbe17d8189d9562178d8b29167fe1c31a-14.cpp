static inline bool tsc_page_update_unsafe(struct kvm_hv *hv)
{
	return (hv->hv_tsc_page_status != HV_TSC_PAGE_GUEST_CHANGED) &&
		hv->hv_tsc_emulation_control;
}