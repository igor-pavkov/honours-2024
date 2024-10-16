static u64 kvm_hv_flush_tlb(struct kvm_vcpu *vcpu, struct kvm_hv_hcall *hc)
{
	struct kvm *kvm = vcpu->kvm;
	struct hv_tlb_flush_ex flush_ex;
	struct hv_tlb_flush flush;
	DECLARE_BITMAP(vcpu_mask, KVM_MAX_VCPUS);
	u64 valid_bank_mask;
	u64 sparse_banks[KVM_HV_MAX_SPARSE_VCPU_SET_BITS];
	bool all_cpus;

	/*
	 * The Hyper-V TLFS doesn't allow more than 64 sparse banks, e.g. the
	 * valid mask is a u64.  Fail the build if KVM's max allowed number of
	 * vCPUs (>4096) would exceed this limit, KVM will additional changes
	 * for Hyper-V support to avoid setting the guest up to fail.
	 */
	BUILD_BUG_ON(KVM_HV_MAX_SPARSE_VCPU_SET_BITS > 64);

	if (hc->code == HVCALL_FLUSH_VIRTUAL_ADDRESS_LIST ||
	    hc->code == HVCALL_FLUSH_VIRTUAL_ADDRESS_SPACE) {
		if (hc->fast) {
			flush.address_space = hc->ingpa;
			flush.flags = hc->outgpa;
			flush.processor_mask = sse128_lo(hc->xmm[0]);
		} else {
			if (unlikely(kvm_read_guest(kvm, hc->ingpa,
						    &flush, sizeof(flush))))
				return HV_STATUS_INVALID_HYPERCALL_INPUT;
		}

		trace_kvm_hv_flush_tlb(flush.processor_mask,
				       flush.address_space, flush.flags);

		valid_bank_mask = BIT_ULL(0);
		sparse_banks[0] = flush.processor_mask;

		/*
		 * Work around possible WS2012 bug: it sends hypercalls
		 * with processor_mask = 0x0 and HV_FLUSH_ALL_PROCESSORS clear,
		 * while also expecting us to flush something and crashing if
		 * we don't. Let's treat processor_mask == 0 same as
		 * HV_FLUSH_ALL_PROCESSORS.
		 */
		all_cpus = (flush.flags & HV_FLUSH_ALL_PROCESSORS) ||
			flush.processor_mask == 0;
	} else {
		if (hc->fast) {
			flush_ex.address_space = hc->ingpa;
			flush_ex.flags = hc->outgpa;
			memcpy(&flush_ex.hv_vp_set,
			       &hc->xmm[0], sizeof(hc->xmm[0]));
		} else {
			if (unlikely(kvm_read_guest(kvm, hc->ingpa, &flush_ex,
						    sizeof(flush_ex))))
				return HV_STATUS_INVALID_HYPERCALL_INPUT;
		}

		trace_kvm_hv_flush_tlb_ex(flush_ex.hv_vp_set.valid_bank_mask,
					  flush_ex.hv_vp_set.format,
					  flush_ex.address_space,
					  flush_ex.flags);

		valid_bank_mask = flush_ex.hv_vp_set.valid_bank_mask;
		all_cpus = flush_ex.hv_vp_set.format !=
			HV_GENERIC_SET_SPARSE_4K;

		if (hc->var_cnt != bitmap_weight((unsigned long *)&valid_bank_mask, 64))
			return HV_STATUS_INVALID_HYPERCALL_INPUT;

		if (all_cpus)
			goto do_flush;

		if (!hc->var_cnt)
			goto ret_success;

		if (kvm_get_sparse_vp_set(kvm, hc, 2, sparse_banks,
					  offsetof(struct hv_tlb_flush_ex,
						   hv_vp_set.bank_contents)))
			return HV_STATUS_INVALID_HYPERCALL_INPUT;
	}

do_flush:
	/*
	 * vcpu->arch.cr3 may not be up-to-date for running vCPUs so we can't
	 * analyze it here, flush TLB regardless of the specified address space.
	 */
	if (all_cpus) {
		kvm_make_all_cpus_request(kvm, KVM_REQ_TLB_FLUSH_GUEST);
	} else {
		sparse_set_to_vcpu_mask(kvm, sparse_banks, valid_bank_mask, vcpu_mask);

		kvm_make_vcpus_request_mask(kvm, KVM_REQ_TLB_FLUSH_GUEST, vcpu_mask);
	}

ret_success:
	/* We always do full TLB flush, set 'Reps completed' = 'Rep Count' */
	return (u64)HV_STATUS_SUCCESS |
		((u64)hc->rep_cnt << HV_HYPERCALL_REP_COMP_OFFSET);
}