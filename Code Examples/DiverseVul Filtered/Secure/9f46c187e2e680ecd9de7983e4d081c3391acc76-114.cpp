static int direct_page_fault(struct kvm_vcpu *vcpu, struct kvm_page_fault *fault)
{
	bool is_tdp_mmu_fault = is_tdp_mmu(vcpu->arch.mmu);

	unsigned long mmu_seq;
	int r;

	fault->gfn = fault->addr >> PAGE_SHIFT;
	fault->slot = kvm_vcpu_gfn_to_memslot(vcpu, fault->gfn);

	if (page_fault_handle_page_track(vcpu, fault))
		return RET_PF_EMULATE;

	r = fast_page_fault(vcpu, fault);
	if (r != RET_PF_INVALID)
		return r;

	r = mmu_topup_memory_caches(vcpu, false);
	if (r)
		return r;

	mmu_seq = vcpu->kvm->mmu_notifier_seq;
	smp_rmb();

	if (kvm_faultin_pfn(vcpu, fault, &r))
		return r;

	if (handle_abnormal_pfn(vcpu, fault, ACC_ALL, &r))
		return r;

	r = RET_PF_RETRY;

	if (is_tdp_mmu_fault)
		read_lock(&vcpu->kvm->mmu_lock);
	else
		write_lock(&vcpu->kvm->mmu_lock);

	if (is_page_fault_stale(vcpu, fault, mmu_seq))
		goto out_unlock;

	r = make_mmu_pages_available(vcpu);
	if (r)
		goto out_unlock;

	if (is_tdp_mmu_fault)
		r = kvm_tdp_mmu_map(vcpu, fault);
	else
		r = __direct_map(vcpu, fault);

out_unlock:
	if (is_tdp_mmu_fault)
		read_unlock(&vcpu->kvm->mmu_lock);
	else
		write_unlock(&vcpu->kvm->mmu_lock);
	kvm_release_pfn_clean(fault->pfn);
	return r;
}