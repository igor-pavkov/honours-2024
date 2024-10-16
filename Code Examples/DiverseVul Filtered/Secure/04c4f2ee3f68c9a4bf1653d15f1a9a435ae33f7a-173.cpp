u64 construct_eptp(struct kvm_vcpu *vcpu, unsigned long root_hpa,
		   int root_level)
{
	u64 eptp = VMX_EPTP_MT_WB;

	eptp |= (root_level == 5) ? VMX_EPTP_PWL_5 : VMX_EPTP_PWL_4;

	if (enable_ept_ad_bits &&
	    (!is_guest_mode(vcpu) || nested_ept_ad_enabled(vcpu)))
		eptp |= VMX_EPTP_AD_ENABLE_BIT;
	eptp |= (root_hpa & PAGE_MASK);

	return eptp;
}