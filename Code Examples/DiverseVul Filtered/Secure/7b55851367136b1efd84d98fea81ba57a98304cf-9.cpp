void __mmdrop(struct mm_struct *mm)
{
	BUG_ON(mm == &init_mm);
	WARN_ON_ONCE(mm == current->mm);
	WARN_ON_ONCE(mm == current->active_mm);
	mm_free_pgd(mm);
	destroy_context(mm);
	hmm_mm_destroy(mm);
	mmu_notifier_mm_destroy(mm);
	check_mm(mm);
	put_user_ns(mm->user_ns);
	free_mm(mm);
}