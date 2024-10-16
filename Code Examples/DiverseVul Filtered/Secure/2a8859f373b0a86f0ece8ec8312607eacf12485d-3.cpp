static void FNAME(invlpg)(struct kvm_vcpu *vcpu, gva_t gva, hpa_t root_hpa)
{
	struct kvm_shadow_walk_iterator iterator;
	struct kvm_mmu_page *sp;
	u64 old_spte;
	int level;
	u64 *sptep;

	vcpu_clear_mmio_info(vcpu, gva);

	/*
	 * No need to check return value here, rmap_can_add() can
	 * help us to skip pte prefetch later.
	 */
	mmu_topup_memory_caches(vcpu, true);

	if (!VALID_PAGE(root_hpa)) {
		WARN_ON(1);
		return;
	}

	write_lock(&vcpu->kvm->mmu_lock);
	for_each_shadow_entry_using_root(vcpu, root_hpa, gva, iterator) {
		level = iterator.level;
		sptep = iterator.sptep;

		sp = sptep_to_sp(sptep);
		old_spte = *sptep;
		if (is_last_spte(old_spte, level)) {
			pt_element_t gpte;
			gpa_t pte_gpa;

			if (!sp->unsync)
				break;

			pte_gpa = FNAME(get_level1_sp_gpa)(sp);
			pte_gpa += (sptep - sp->spt) * sizeof(pt_element_t);

			mmu_page_zap_pte(vcpu->kvm, sp, sptep, NULL);
			if (is_shadow_present_pte(old_spte))
				kvm_flush_remote_tlbs_with_address(vcpu->kvm,
					sp->gfn, KVM_PAGES_PER_HPAGE(sp->role.level));

			if (!rmap_can_add(vcpu))
				break;

			if (kvm_vcpu_read_guest_atomic(vcpu, pte_gpa, &gpte,
						       sizeof(pt_element_t)))
				break;

			FNAME(prefetch_gpte)(vcpu, sp, sptep, gpte, false);
		}

		if (!sp->unsync_children)
			break;
	}
	write_unlock(&vcpu->kvm->mmu_lock);
}