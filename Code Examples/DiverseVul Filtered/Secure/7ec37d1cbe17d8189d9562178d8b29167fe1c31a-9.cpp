static void synic_update_vector(struct kvm_vcpu_hv_synic *synic,
				int vector)
{
	struct kvm_vcpu *vcpu = hv_synic_to_vcpu(synic);
	struct kvm_hv *hv = to_kvm_hv(vcpu->kvm);
	int auto_eoi_old, auto_eoi_new;

	if (vector < HV_SYNIC_FIRST_VALID_VECTOR)
		return;

	if (synic_has_vector_connected(synic, vector))
		__set_bit(vector, synic->vec_bitmap);
	else
		__clear_bit(vector, synic->vec_bitmap);

	auto_eoi_old = bitmap_weight(synic->auto_eoi_bitmap, 256);

	if (synic_has_vector_auto_eoi(synic, vector))
		__set_bit(vector, synic->auto_eoi_bitmap);
	else
		__clear_bit(vector, synic->auto_eoi_bitmap);

	auto_eoi_new = bitmap_weight(synic->auto_eoi_bitmap, 256);

	if (!!auto_eoi_old == !!auto_eoi_new)
		return;

	if (!enable_apicv)
		return;

	down_write(&vcpu->kvm->arch.apicv_update_lock);

	if (auto_eoi_new)
		hv->synic_auto_eoi_used++;
	else
		hv->synic_auto_eoi_used--;

	__kvm_request_apicv_update(vcpu->kvm,
				   !hv->synic_auto_eoi_used,
				   APICV_INHIBIT_REASON_HYPERV);

	up_write(&vcpu->kvm->arch.apicv_update_lock);
}