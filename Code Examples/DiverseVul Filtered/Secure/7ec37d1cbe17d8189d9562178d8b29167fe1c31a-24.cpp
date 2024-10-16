static u64 get_time_ref_counter(struct kvm *kvm)
{
	struct kvm_hv *hv = to_kvm_hv(kvm);
	struct kvm_vcpu *vcpu;
	u64 tsc;

	/*
	 * Fall back to get_kvmclock_ns() when TSC page hasn't been set up,
	 * is broken, disabled or being updated.
	 */
	if (hv->hv_tsc_page_status != HV_TSC_PAGE_SET)
		return div_u64(get_kvmclock_ns(kvm), 100);

	vcpu = kvm_get_vcpu(kvm, 0);
	tsc = kvm_read_l1_tsc(vcpu, rdtsc());
	return mul_u64_u64_shr(tsc, hv->tsc_ref.tsc_scale, 64)
		+ hv->tsc_ref.tsc_offset;
}