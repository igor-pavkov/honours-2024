int kvm_lapic_set_pv_eoi(struct kvm_vcpu *vcpu, u64 data, unsigned long len)
{
	u64 addr = data & ~KVM_MSR_ENABLED;
	struct gfn_to_hva_cache *ghc = &vcpu->arch.pv_eoi.data;
	unsigned long new_len;
	int ret;

	if (!IS_ALIGNED(addr, 4))
		return 1;

	if (data & KVM_MSR_ENABLED) {
		if (addr == ghc->gpa && len <= ghc->len)
			new_len = ghc->len;
		else
			new_len = len;

		ret = kvm_gfn_to_hva_cache_init(vcpu->kvm, ghc, addr, new_len);
		if (ret)
			return ret;
	}

	vcpu->arch.pv_eoi.msr_val = data;

	return 0;
}