static int do_set_msr(struct kvm_vcpu *vcpu, unsigned index, u64 *data)
{
	struct msr_data msr;

	msr.data = *data;
	msr.index = index;
	msr.host_initiated = true;
	return kvm_set_msr(vcpu, &msr);
}