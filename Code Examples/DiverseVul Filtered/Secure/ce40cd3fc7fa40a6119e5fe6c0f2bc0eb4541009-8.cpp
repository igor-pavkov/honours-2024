static inline bool kvm_hv_vapic_assist_page_enabled(struct kvm_vcpu *vcpu)
{
	return vcpu->arch.hv_vapic & HV_X64_MSR_APIC_ASSIST_PAGE_ENABLE;
}