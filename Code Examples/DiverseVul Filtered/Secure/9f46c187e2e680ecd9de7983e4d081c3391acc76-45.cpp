static bool __kvm_mmu_prepare_zap_page(struct kvm *kvm,
				       struct kvm_mmu_page *sp,
				       struct list_head *invalid_list,
				       int *nr_zapped)
{
	bool list_unstable, zapped_root = false;

	trace_kvm_mmu_prepare_zap_page(sp);
	++kvm->stat.mmu_shadow_zapped;
	*nr_zapped = mmu_zap_unsync_children(kvm, sp, invalid_list);
	*nr_zapped += kvm_mmu_page_unlink_children(kvm, sp, invalid_list);
	kvm_mmu_unlink_parents(sp);

	/* Zapping children means active_mmu_pages has become unstable. */
	list_unstable = *nr_zapped;

	if (!sp->role.invalid && !sp->role.direct)
		unaccount_shadowed(kvm, sp);

	if (sp->unsync)
		kvm_unlink_unsync_page(kvm, sp);
	if (!sp->root_count) {
		/* Count self */
		(*nr_zapped)++;

		/*
		 * Already invalid pages (previously active roots) are not on
		 * the active page list.  See list_del() in the "else" case of
		 * !sp->root_count.
		 */
		if (sp->role.invalid)
			list_add(&sp->link, invalid_list);
		else
			list_move(&sp->link, invalid_list);
		kvm_mod_used_mmu_pages(kvm, -1);
	} else {
		/*
		 * Remove the active root from the active page list, the root
		 * will be explicitly freed when the root_count hits zero.
		 */
		list_del(&sp->link);

		/*
		 * Obsolete pages cannot be used on any vCPUs, see the comment
		 * in kvm_mmu_zap_all_fast().  Note, is_obsolete_sp() also
		 * treats invalid shadow pages as being obsolete.
		 */
		zapped_root = !is_obsolete_sp(kvm, sp);
	}

	if (sp->lpage_disallowed)
		unaccount_huge_nx_page(kvm, sp);

	sp->role.invalid = 1;

	/*
	 * Make the request to free obsolete roots after marking the root
	 * invalid, otherwise other vCPUs may not see it as invalid.
	 */
	if (zapped_root)
		kvm_make_all_cpus_request(kvm, KVM_REQ_MMU_FREE_OBSOLETE_ROOTS);
	return list_unstable;
}