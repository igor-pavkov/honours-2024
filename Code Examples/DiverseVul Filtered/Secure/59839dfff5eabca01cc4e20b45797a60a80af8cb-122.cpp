int kvm_arch_vcpu_runnable(struct kvm_vcpu *vcpu)
{
	return vcpu->arch.mp_state == KVM_MP_STATE_RUNNABLE
	       || vcpu->arch.mp_state == KVM_MP_STATE_SIPI_RECEIVED
	       || vcpu->arch.nmi_pending;
}