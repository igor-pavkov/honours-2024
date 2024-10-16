static __init void ftrace_profile_debugfs(struct dentry *d_tracer)
{
	struct ftrace_profile_stat *stat;
	struct dentry *entry;
	char *name;
	int ret;
	int cpu;

	for_each_possible_cpu(cpu) {
		stat = &per_cpu(ftrace_profile_stats, cpu);

		/* allocate enough for function name + cpu number */
		name = kmalloc(32, GFP_KERNEL);
		if (!name) {
			/*
			 * The files created are permanent, if something happens
			 * we still do not free memory.
			 */
			WARN(1,
			     "Could not allocate stat file for cpu %d\n",
			     cpu);
			return;
		}
		stat->stat = function_stats;
		snprintf(name, 32, "function%d", cpu);
		stat->stat.name = name;
		ret = register_stat_tracer(&stat->stat);
		if (ret) {
			WARN(1,
			     "Could not register function stat for cpu %d\n",
			     cpu);
			kfree(name);
			return;
		}
	}

	entry = debugfs_create_file("function_profile_enabled", 0644,
				    d_tracer, NULL, &ftrace_profile_fops);
	if (!entry)
		pr_warning("Could not create debugfs "
			   "'function_profile_enabled' entry\n");
}