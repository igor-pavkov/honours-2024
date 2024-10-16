static u64 kvm_get_sparse_vp_set(struct kvm *kvm, struct kvm_hv_hcall *hc,
				 int consumed_xmm_halves,
				 u64 *sparse_banks, gpa_t offset)
{
	u16 var_cnt;
	int i;

	if (hc->var_cnt > 64)
		return -EINVAL;

	/* Ignore banks that cannot possibly contain a legal VP index. */
	var_cnt = min_t(u16, hc->var_cnt, KVM_HV_MAX_SPARSE_VCPU_SET_BITS);

	if (hc->fast) {
		/*
		 * Each XMM holds two sparse banks, but do not count halves that
		 * have already been consumed for hypercall parameters.
		 */
		if (hc->var_cnt > 2 * HV_HYPERCALL_MAX_XMM_REGISTERS - consumed_xmm_halves)
			return HV_STATUS_INVALID_HYPERCALL_INPUT;
		for (i = 0; i < var_cnt; i++) {
			int j = i + consumed_xmm_halves;
			if (j % 2)
				sparse_banks[i] = sse128_hi(hc->xmm[j / 2]);
			else
				sparse_banks[i] = sse128_lo(hc->xmm[j / 2]);
		}
		return 0;
	}

	return kvm_read_guest(kvm, hc->ingpa + offset, sparse_banks,
			      var_cnt * sizeof(*sparse_banks));
}