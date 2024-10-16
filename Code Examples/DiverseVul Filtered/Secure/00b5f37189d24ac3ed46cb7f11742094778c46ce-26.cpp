static void pv_eoi_set_pending(struct kvm_vcpu *vcpu)
{
	if (pv_eoi_put_user(vcpu, KVM_PV_EOI_ENABLED) < 0)
		return;

	__set_bit(KVM_APIC_PV_EOI_PENDING, &vcpu->arch.apic_attention);
}