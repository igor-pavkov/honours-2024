void kvm_make_mclock_inprogress_request(struct kvm *kvm)
{
	make_all_cpus_request(kvm, KVM_REQ_MCLOCK_INPROGRESS);
}