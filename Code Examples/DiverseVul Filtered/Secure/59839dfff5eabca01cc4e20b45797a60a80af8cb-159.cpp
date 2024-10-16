static int kvm_request_guest_time_update(struct kvm_vcpu *v)
{
	struct kvm_vcpu_arch *vcpu = &v->arch;

	if (!vcpu->time_page)
		return 0;
	set_bit(KVM_REQ_KVMCLOCK_UPDATE, &v->requests);
	return 1;
}