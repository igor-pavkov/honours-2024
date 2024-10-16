static inline bool kvm_guest_apic_has_interrupt(struct kvm_vcpu *vcpu)
{
	return (is_guest_mode(vcpu) &&
			kvm_x86_ops->guest_apic_has_interrupt &&
			kvm_x86_ops->guest_apic_has_interrupt(vcpu));
}