static int FNAME(page_fault)(struct kvm_vcpu *vcpu, struct kvm_page_fault *fault)
{
	struct guest_walker walker;
	int r;
	unsigned long mmu_seq;
	bool is_self_change_mapping;

	pgprintk("%s: addr %lx err %x\n", __func__, fault->addr, fault->error_code);
	WARN_ON_ONCE(fault->is_tdp);

	/*
	 * Look up the guest pte for the faulting address.
	 * If PFEC.RSVD is set, this is a shadow page fault.
	 * The bit needs to be cleared before walking guest page tables.
	 */
	r = FNAME(walk_addr)(&walker, vcpu, fault->addr,
			     fault->error_code & ~PFERR_RSVD_MASK);

	/*
	 * The page is not mapped by the guest.  Let the guest handle it.
	 */
	if (!r) {
		pgprintk("%s: guest page fault\n", __func__);
		if (!fault->prefetch)
			kvm_inject_emulated_page_fault(vcpu, &walker.fault);

		return RET_PF_RETRY;
	}

	fault->gfn = walker.gfn;
	fault->slot = kvm_vcpu_gfn_to_memslot(vcpu, fault->gfn);

	if (page_fault_handle_page_track(vcpu, fault)) {
		shadow_page_table_clear_flood(vcpu, fault->addr);
		return RET_PF_EMULATE;
	}

	r = mmu_topup_memory_caches(vcpu, true);
	if (r)
		return r;

	vcpu->arch.write_fault_to_shadow_pgtable = false;

	is_self_change_mapping = FNAME(is_self_change_mapping)(vcpu,
	      &walker, fault->user, &vcpu->arch.write_fault_to_shadow_pgtable);

	if (is_self_change_mapping)
		fault->max_level = PG_LEVEL_4K;
	else
		fault->max_level = walker.level;

	mmu_seq = vcpu->kvm->mmu_notifier_seq;
	smp_rmb();

	if (kvm_faultin_pfn(vcpu, fault, &r))
		return r;

	if (handle_abnormal_pfn(vcpu, fault, walker.pte_access, &r))
		return r;

	/*
	 * Do not change pte_access if the pfn is a mmio page, otherwise
	 * we will cache the incorrect access into mmio spte.
	 */
	if (fault->write && !(walker.pte_access & ACC_WRITE_MASK) &&
	    !is_cr0_wp(vcpu->arch.mmu) && !fault->user && fault->slot) {
		walker.pte_access |= ACC_WRITE_MASK;
		walker.pte_access &= ~ACC_USER_MASK;

		/*
		 * If we converted a user page to a kernel page,
		 * so that the kernel can write to it when cr0.wp=0,
		 * then we should prevent the kernel from executing it
		 * if SMEP is enabled.
		 */
		if (is_cr4_smep(vcpu->arch.mmu))
			walker.pte_access &= ~ACC_EXEC_MASK;
	}

	r = RET_PF_RETRY;
	write_lock(&vcpu->kvm->mmu_lock);

	if (is_page_fault_stale(vcpu, fault, mmu_seq))
		goto out_unlock;

	r = make_mmu_pages_available(vcpu);
	if (r)
		goto out_unlock;
	r = FNAME(fetch)(vcpu, fault, &walker);

out_unlock:
	write_unlock(&vcpu->kvm->mmu_lock);
	kvm_release_pfn_clean(fault->pfn);
	return r;
}