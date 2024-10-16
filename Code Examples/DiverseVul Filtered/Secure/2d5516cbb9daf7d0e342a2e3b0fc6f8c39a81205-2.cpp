static struct mm_struct * mm_init(struct mm_struct * mm, struct task_struct *p)
{
	atomic_set(&mm->mm_users, 1);
	atomic_set(&mm->mm_count, 1);
	init_rwsem(&mm->mmap_sem);
	INIT_LIST_HEAD(&mm->mmlist);
	mm->flags = (current->mm) ? current->mm->flags : default_dump_filter;
	mm->core_state = NULL;
	mm->nr_ptes = 0;
	set_mm_counter(mm, file_rss, 0);
	set_mm_counter(mm, anon_rss, 0);
	spin_lock_init(&mm->page_table_lock);
	spin_lock_init(&mm->ioctx_lock);
	INIT_HLIST_HEAD(&mm->ioctx_list);
	mm->free_area_cache = TASK_UNMAPPED_BASE;
	mm->cached_hole_size = ~0UL;
	mm_init_owner(mm, p);

	if (likely(!mm_alloc_pgd(mm))) {
		mm->def_flags = 0;
		mmu_notifier_mm_init(mm);
		return mm;
	}

	free_mm(mm);
	return NULL;
}