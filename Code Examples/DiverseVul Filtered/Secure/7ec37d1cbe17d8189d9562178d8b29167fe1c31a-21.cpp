void kvm_hv_invalidate_tsc_page(struct kvm *kvm)
{
	struct kvm_hv *hv = to_kvm_hv(kvm);
	u64 gfn;
	int idx;

	if (hv->hv_tsc_page_status == HV_TSC_PAGE_BROKEN ||
	    hv->hv_tsc_page_status == HV_TSC_PAGE_UNSET ||
	    tsc_page_update_unsafe(hv))
		return;

	mutex_lock(&hv->hv_lock);

	if (!(hv->hv_tsc_page & HV_X64_MSR_TSC_REFERENCE_ENABLE))
		goto out_unlock;

	/* Preserve HV_TSC_PAGE_GUEST_CHANGED/HV_TSC_PAGE_HOST_CHANGED states */
	if (hv->hv_tsc_page_status == HV_TSC_PAGE_SET)
		hv->hv_tsc_page_status = HV_TSC_PAGE_UPDATING;

	gfn = hv->hv_tsc_page >> HV_X64_MSR_TSC_REFERENCE_ADDRESS_SHIFT;

	hv->tsc_ref.tsc_sequence = 0;

	/*
	 * Take the srcu lock as memslots will be accessed to check the gfn
	 * cache generation against the memslots generation.
	 */
	idx = srcu_read_lock(&kvm->srcu);
	if (kvm_write_guest(kvm, gfn_to_gpa(gfn),
			    &hv->tsc_ref, sizeof(hv->tsc_ref.tsc_sequence)))
		hv->hv_tsc_page_status = HV_TSC_PAGE_BROKEN;
	srcu_read_unlock(&kvm->srcu, idx);

out_unlock:
	mutex_unlock(&hv->hv_lock);
}