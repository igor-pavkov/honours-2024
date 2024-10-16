static int __init ftrace_dyn_table_alloc(unsigned long num_to_init)
{
	int cnt;

	if (!num_to_init) {
		pr_info("ftrace: No functions to be traced?\n");
		return -1;
	}

	cnt = num_to_init / ENTRIES_PER_PAGE;
	pr_info("ftrace: allocating %ld entries in %d pages\n",
		num_to_init, cnt + 1);

	return 0;
}