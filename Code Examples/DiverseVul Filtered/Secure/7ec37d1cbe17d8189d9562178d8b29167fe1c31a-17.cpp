static u64 kvm_hv_send_ipi(struct kvm_vcpu *vcpu, struct kvm_hv_hcall *hc)
{
	struct kvm *kvm = vcpu->kvm;
	struct hv_send_ipi_ex send_ipi_ex;
	struct hv_send_ipi send_ipi;
	DECLARE_BITMAP(vcpu_mask, KVM_MAX_VCPUS);
	unsigned long valid_bank_mask;
	u64 sparse_banks[KVM_HV_MAX_SPARSE_VCPU_SET_BITS];
	u32 vector;
	bool all_cpus;

	if (hc->code == HVCALL_SEND_IPI) {
		if (!hc->fast) {
			if (unlikely(kvm_read_guest(kvm, hc->ingpa, &send_ipi,
						    sizeof(send_ipi))))
				return HV_STATUS_INVALID_HYPERCALL_INPUT;
			sparse_banks[0] = send_ipi.cpu_mask;
			vector = send_ipi.vector;
		} else {
			/* 'reserved' part of hv_send_ipi should be 0 */
			if (unlikely(hc->ingpa >> 32 != 0))
				return HV_STATUS_INVALID_HYPERCALL_INPUT;
			sparse_banks[0] = hc->outgpa;
			vector = (u32)hc->ingpa;
		}
		all_cpus = false;
		valid_bank_mask = BIT_ULL(0);

		trace_kvm_hv_send_ipi(vector, sparse_banks[0]);
	} else {
		if (!hc->fast) {
			if (unlikely(kvm_read_guest(kvm, hc->ingpa, &send_ipi_ex,
						    sizeof(send_ipi_ex))))
				return HV_STATUS_INVALID_HYPERCALL_INPUT;
		} else {
			send_ipi_ex.vector = (u32)hc->ingpa;
			send_ipi_ex.vp_set.format = hc->outgpa;
			send_ipi_ex.vp_set.valid_bank_mask = sse128_lo(hc->xmm[0]);
		}

		trace_kvm_hv_send_ipi_ex(send_ipi_ex.vector,
					 send_ipi_ex.vp_set.format,
					 send_ipi_ex.vp_set.valid_bank_mask);

		vector = send_ipi_ex.vector;
		valid_bank_mask = send_ipi_ex.vp_set.valid_bank_mask;
		all_cpus = send_ipi_ex.vp_set.format == HV_GENERIC_SET_ALL;

		if (hc->var_cnt != bitmap_weight(&valid_bank_mask, 64))
			return HV_STATUS_INVALID_HYPERCALL_INPUT;

		if (all_cpus)
			goto check_and_send_ipi;

		if (!hc->var_cnt)
			goto ret_success;

		if (kvm_get_sparse_vp_set(kvm, hc, 1, sparse_banks,
					  offsetof(struct hv_send_ipi_ex,
						   vp_set.bank_contents)))
			return HV_STATUS_INVALID_HYPERCALL_INPUT;
	}

check_and_send_ipi:
	if ((vector < HV_IPI_LOW_VECTOR) || (vector > HV_IPI_HIGH_VECTOR))
		return HV_STATUS_INVALID_HYPERCALL_INPUT;

	if (all_cpus) {
		kvm_send_ipi_to_many(kvm, vector, NULL);
	} else {
		sparse_set_to_vcpu_mask(kvm, sparse_banks, valid_bank_mask, vcpu_mask);

		kvm_send_ipi_to_many(kvm, vector, vcpu_mask);
	}

ret_success:
	return HV_STATUS_SUCCESS;
}