int kvm_hv_set_enforce_cpuid(struct kvm_vcpu *vcpu, bool enforce)
{
	struct kvm_vcpu_hv *hv_vcpu;
	int ret = 0;

	if (!to_hv_vcpu(vcpu)) {
		if (enforce) {
			ret = kvm_hv_vcpu_init(vcpu);
			if (ret)
				return ret;
		} else {
			return 0;
		}
	}

	hv_vcpu = to_hv_vcpu(vcpu);
	hv_vcpu->enforce_cpuid = enforce;

	return ret;
}