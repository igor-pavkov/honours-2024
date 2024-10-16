bool kvm_make_all_cpus_request(struct kvm *kvm, unsigned int req)
{
	return kvm_make_all_cpus_request_except(kvm, req, NULL);
}