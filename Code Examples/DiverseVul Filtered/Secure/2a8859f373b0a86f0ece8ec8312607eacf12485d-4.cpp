FNAME(prefetch_gpte)(struct kvm_vcpu *vcpu, struct kvm_mmu_page *sp,
		     u64 *spte, pt_element_t gpte, bool no_dirty_log)
{
	struct kvm_memory_slot *slot;
	unsigned pte_access;
	gfn_t gfn;
	kvm_pfn_t pfn;

	if (FNAME(prefetch_invalid_gpte)(vcpu, sp, spte, gpte))
		return false;

	pgprintk("%s: gpte %llx spte %p\n", __func__, (u64)gpte, spte);

	gfn = gpte_to_gfn(gpte);
	pte_access = sp->role.access & FNAME(gpte_access)(gpte);
	FNAME(protect_clean_gpte)(vcpu->arch.mmu, &pte_access, gpte);

	slot = gfn_to_memslot_dirty_bitmap(vcpu, gfn,
			no_dirty_log && (pte_access & ACC_WRITE_MASK));
	if (!slot)
		return false;

	pfn = gfn_to_pfn_memslot_atomic(slot, gfn);
	if (is_error_pfn(pfn))
		return false;

	mmu_set_spte(vcpu, slot, spte, pte_access, gfn, pfn, NULL);
	kvm_release_pfn_clean(pfn);
	return true;
}