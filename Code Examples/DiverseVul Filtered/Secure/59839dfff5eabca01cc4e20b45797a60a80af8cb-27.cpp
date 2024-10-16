int kvm_arch_vcpu_ioctl_set_mpstate(struct kvm_vcpu *vcpu,
				    struct kvm_mp_state *mp_state)
{
	vcpu_load(vcpu);
	vcpu->arch.mp_state = mp_state->mp_state;
	vcpu_put(vcpu);
	return 0;
}