void kvm_lapic_set_tpr(struct kvm_vcpu *vcpu, unsigned long cr8)
{
	apic_set_tpr(vcpu->arch.apic, (cr8 & 0x0f) << 4);
}