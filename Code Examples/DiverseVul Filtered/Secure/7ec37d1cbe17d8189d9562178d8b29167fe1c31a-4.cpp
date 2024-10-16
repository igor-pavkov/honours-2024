int kvm_hv_activate_synic(struct kvm_vcpu *vcpu, bool dont_zero_synic_pages)
{
	struct kvm_vcpu_hv_synic *synic;
	int r;

	if (!to_hv_vcpu(vcpu)) {
		r = kvm_hv_vcpu_init(vcpu);
		if (r)
			return r;
	}

	synic = to_hv_synic(vcpu);

	synic->active = true;
	synic->dont_zero_synic_pages = dont_zero_synic_pages;
	synic->control = HV_SYNIC_CONTROL_ENABLE;
	return 0;
}