static u16 kvm_hvcall_signal_event(struct kvm_vcpu *vcpu, struct kvm_hv_hcall *hc)
{
	struct kvm_hv *hv = to_kvm_hv(vcpu->kvm);
	struct eventfd_ctx *eventfd;

	if (unlikely(!hc->fast)) {
		int ret;
		gpa_t gpa = hc->ingpa;

		if ((gpa & (__alignof__(hc->ingpa) - 1)) ||
		    offset_in_page(gpa) + sizeof(hc->ingpa) > PAGE_SIZE)
			return HV_STATUS_INVALID_ALIGNMENT;

		ret = kvm_vcpu_read_guest(vcpu, gpa,
					  &hc->ingpa, sizeof(hc->ingpa));
		if (ret < 0)
			return HV_STATUS_INVALID_ALIGNMENT;
	}

	/*
	 * Per spec, bits 32-47 contain the extra "flag number".  However, we
	 * have no use for it, and in all known usecases it is zero, so just
	 * report lookup failure if it isn't.
	 */
	if (hc->ingpa & 0xffff00000000ULL)
		return HV_STATUS_INVALID_PORT_ID;
	/* remaining bits are reserved-zero */
	if (hc->ingpa & ~KVM_HYPERV_CONN_ID_MASK)
		return HV_STATUS_INVALID_HYPERCALL_INPUT;

	/* the eventfd is protected by vcpu->kvm->srcu, but conn_to_evt isn't */
	rcu_read_lock();
	eventfd = idr_find(&hv->conn_to_evt, hc->ingpa);
	rcu_read_unlock();
	if (!eventfd)
		return HV_STATUS_INVALID_PORT_ID;

	eventfd_signal(eventfd, 1);
	return HV_STATUS_SUCCESS;
}