static bool kvm_can_post_timer_interrupt(struct kvm_vcpu *vcpu)
{
	return pi_inject_timer && kvm_vcpu_apicv_active(vcpu) &&
		(kvm_mwait_in_guest(vcpu->kvm) || kvm_hlt_in_guest(vcpu->kvm));
}