static inline bool kvm_apic_present(struct kvm_vcpu *vcpu)
{
	return kvm_vcpu_has_lapic(vcpu) && kvm_apic_hw_enabled(vcpu->arch.apic);
}