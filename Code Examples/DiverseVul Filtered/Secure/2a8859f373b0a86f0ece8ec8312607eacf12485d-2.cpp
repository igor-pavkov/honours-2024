static int FNAME(sync_page)(struct kvm_vcpu *vcpu, struct kvm_mmu_page *sp)
{
	union kvm_mmu_page_role mmu_role = vcpu->arch.mmu->mmu_role.base;
	int i;
	bool host_writable;
	gpa_t first_pte_gpa;
	bool flush = false;

	/*
	 * Ignore various flags when verifying that it's safe to sync a shadow
	 * page using the current MMU context.
	 *
	 *  - level: not part of the overall MMU role and will never match as the MMU's
	 *           level tracks the root level
	 *  - access: updated based on the new guest PTE
	 *  - quadrant: not part of the overall MMU role (similar to level)
	 */
	const union kvm_mmu_page_role sync_role_ign = {
		.level = 0xf,
		.access = 0x7,
		.quadrant = 0x3,
	};

	/*
	 * Direct pages can never be unsync, and KVM should never attempt to
	 * sync a shadow page for a different MMU context, e.g. if the role
	 * differs then the memslot lookup (SMM vs. non-SMM) will be bogus, the
	 * reserved bits checks will be wrong, etc...
	 */
	if (WARN_ON_ONCE(sp->role.direct ||
			 (sp->role.word ^ mmu_role.word) & ~sync_role_ign.word))
		return -1;

	first_pte_gpa = FNAME(get_level1_sp_gpa)(sp);

	for (i = 0; i < PT64_ENT_PER_PAGE; i++) {
		u64 *sptep, spte;
		struct kvm_memory_slot *slot;
		unsigned pte_access;
		pt_element_t gpte;
		gpa_t pte_gpa;
		gfn_t gfn;

		if (!sp->spt[i])
			continue;

		pte_gpa = first_pte_gpa + i * sizeof(pt_element_t);

		if (kvm_vcpu_read_guest_atomic(vcpu, pte_gpa, &gpte,
					       sizeof(pt_element_t)))
			return -1;

		if (FNAME(prefetch_invalid_gpte)(vcpu, sp, &sp->spt[i], gpte)) {
			flush = true;
			continue;
		}

		gfn = gpte_to_gfn(gpte);
		pte_access = sp->role.access;
		pte_access &= FNAME(gpte_access)(gpte);
		FNAME(protect_clean_gpte)(vcpu->arch.mmu, &pte_access, gpte);

		if (sync_mmio_spte(vcpu, &sp->spt[i], gfn, pte_access))
			continue;

		if (gfn != sp->gfns[i]) {
			drop_spte(vcpu->kvm, &sp->spt[i]);
			flush = true;
			continue;
		}

		sptep = &sp->spt[i];
		spte = *sptep;
		host_writable = spte & shadow_host_writable_mask;
		slot = kvm_vcpu_gfn_to_memslot(vcpu, gfn);
		make_spte(vcpu, sp, slot, pte_access, gfn,
			  spte_to_pfn(spte), spte, true, false,
			  host_writable, &spte);

		flush |= mmu_spte_update(sptep, spte);
	}

	return flush;
}