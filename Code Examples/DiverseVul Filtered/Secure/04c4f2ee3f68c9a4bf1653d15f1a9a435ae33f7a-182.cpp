static int hv_remote_flush_tlb(struct kvm *kvm)
{
	return hv_remote_flush_tlb_with_range(kvm, NULL);
}