int kvm_hv_set_msr_common(struct kvm_vcpu *vcpu, u32 msr, u64 data, bool host)
{
	struct kvm_hv *hv = to_kvm_hv(vcpu->kvm);

	if (!host && !vcpu->arch.hyperv_enabled)
		return 1;

	if (!to_hv_vcpu(vcpu)) {
		if (kvm_hv_vcpu_init(vcpu))
			return 1;
	}

	if (kvm_hv_msr_partition_wide(msr)) {
		int r;

		mutex_lock(&hv->hv_lock);
		r = kvm_hv_set_msr_pw(vcpu, msr, data, host);
		mutex_unlock(&hv->hv_lock);
		return r;
	} else
		return kvm_hv_set_msr(vcpu, msr, data, host);
}