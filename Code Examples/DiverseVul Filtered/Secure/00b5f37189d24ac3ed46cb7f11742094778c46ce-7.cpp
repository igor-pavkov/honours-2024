int kvm_hv_vapic_msr_read(struct kvm_vcpu *vcpu, u32 reg, u64 *data)
{
	if (!lapic_in_kernel(vcpu))
		return 1;

	return kvm_lapic_msr_read(vcpu->arch.apic, reg, data);
}