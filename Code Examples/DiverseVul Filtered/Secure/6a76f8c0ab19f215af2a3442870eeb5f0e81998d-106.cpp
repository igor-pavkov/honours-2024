void ftrace_release_mod(struct module *mod)
{
	struct dyn_ftrace *rec;
	struct ftrace_page **last_pg;
	struct ftrace_page *pg;
	int order;

	mutex_lock(&ftrace_lock);

	if (ftrace_disabled)
		goto out_unlock;

	/*
	 * Each module has its own ftrace_pages, remove
	 * them from the list.
	 */
	last_pg = &ftrace_pages_start;
	for (pg = ftrace_pages_start; pg; pg = *last_pg) {
		rec = &pg->records[0];
		if (within_module_core(rec->ip, mod)) {
			/*
			 * As core pages are first, the first
			 * page should never be a module page.
			 */
			if (WARN_ON(pg == ftrace_pages_start))
				goto out_unlock;

			/* Check if we are deleting the last page */
			if (pg == ftrace_pages)
				ftrace_pages = next_to_ftrace_page(last_pg);

			*last_pg = pg->next;
			order = get_count_order(pg->size / ENTRIES_PER_PAGE);
			free_pages((unsigned long)pg->records, order);
			kfree(pg);
		} else
			last_pg = &pg->next;
	}
 out_unlock:
	mutex_unlock(&ftrace_lock);
}