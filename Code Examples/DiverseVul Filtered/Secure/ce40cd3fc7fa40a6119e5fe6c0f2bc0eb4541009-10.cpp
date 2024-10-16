static inline bool kvm_vcpu_has_lapic(struct kvm_vcpu *vcpu)
{
	if (static_key_false(&kvm_no_apic_vcpu))
		return vcpu->arch.apic;
	return true;
}