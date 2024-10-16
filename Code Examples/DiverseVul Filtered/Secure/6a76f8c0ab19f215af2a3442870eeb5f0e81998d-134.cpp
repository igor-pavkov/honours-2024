int ftrace_profile_pages_init(struct ftrace_profile_stat *stat)
{
	struct ftrace_profile_page *pg;
	int functions;
	int pages;
	int i;

	/* If we already allocated, do nothing */
	if (stat->pages)
		return 0;

	stat->pages = (void *)get_zeroed_page(GFP_KERNEL);
	if (!stat->pages)
		return -ENOMEM;

#ifdef CONFIG_DYNAMIC_FTRACE
	functions = ftrace_update_tot_cnt;
#else
	/*
	 * We do not know the number of functions that exist because
	 * dynamic tracing is what counts them. With past experience
	 * we have around 20K functions. That should be more than enough.
	 * It is highly unlikely we will execute every function in
	 * the kernel.
	 */
	functions = 20000;
#endif

	pg = stat->start = stat->pages;

	pages = DIV_ROUND_UP(functions, PROFILES_PER_PAGE);

	for (i = 0; i < pages; i++) {
		pg->next = (void *)get_zeroed_page(GFP_KERNEL);
		if (!pg->next)
			goto out_free;
		pg = pg->next;
	}

	return 0;

 out_free:
	pg = stat->start;
	while (pg) {
		unsigned long tmp = (unsigned long)pg;

		pg = pg->next;
		free_page(tmp);
	}

	stat->pages = NULL;
	stat->start = NULL;

	return -ENOMEM;
}