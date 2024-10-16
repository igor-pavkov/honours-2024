static int do_replace_finish(struct net *net, struct ebt_replace *repl,
			      struct ebt_table_info *newinfo)
{
	int ret, i;
	struct ebt_counter *counterstmp = NULL;
	/* used to be able to unlock earlier */
	struct ebt_table_info *table;
	struct ebt_table *t;

	/* the user wants counters back
	   the check on the size is done later, when we have the lock */
	if (repl->num_counters) {
		unsigned long size = repl->num_counters * sizeof(*counterstmp);
		counterstmp = vmalloc(size);
		if (!counterstmp)
			return -ENOMEM;
	}

	newinfo->chainstack = NULL;
	ret = ebt_verify_pointers(repl, newinfo);
	if (ret != 0)
		goto free_counterstmp;

	ret = translate_table(net, repl->name, newinfo);

	if (ret != 0)
		goto free_counterstmp;

	t = find_table_lock(net, repl->name, &ret, &ebt_mutex);
	if (!t) {
		ret = -ENOENT;
		goto free_iterate;
	}

	/* the table doesn't like it */
	if (t->check && (ret = t->check(newinfo, repl->valid_hooks)))
		goto free_unlock;

	if (repl->num_counters && repl->num_counters != t->private->nentries) {
		BUGPRINT("Wrong nr. of counters requested\n");
		ret = -EINVAL;
		goto free_unlock;
	}

	/* we have the mutex lock, so no danger in reading this pointer */
	table = t->private;
	/* make sure the table can only be rmmod'ed if it contains no rules */
	if (!table->nentries && newinfo->nentries && !try_module_get(t->me)) {
		ret = -ENOENT;
		goto free_unlock;
	} else if (table->nentries && !newinfo->nentries)
		module_put(t->me);
	/* we need an atomic snapshot of the counters */
	write_lock_bh(&t->lock);
	if (repl->num_counters)
		get_counters(t->private->counters, counterstmp,
		   t->private->nentries);

	t->private = newinfo;
	write_unlock_bh(&t->lock);
	mutex_unlock(&ebt_mutex);
	/* so, a user can change the chains while having messed up her counter
	   allocation. Only reason why this is done is because this way the lock
	   is held only once, while this doesn't bring the kernel into a
	   dangerous state. */
	if (repl->num_counters &&
	   copy_to_user(repl->counters, counterstmp,
	   repl->num_counters * sizeof(struct ebt_counter))) {
		ret = -EFAULT;
	}
	else
		ret = 0;

	/* decrease module count and free resources */
	EBT_ENTRY_ITERATE(table->entries, table->entries_size,
			  ebt_cleanup_entry, net, NULL);

	vfree(table->entries);
	if (table->chainstack) {
		for_each_possible_cpu(i)
			vfree(table->chainstack[i]);
		vfree(table->chainstack);
	}
	vfree(table);

	vfree(counterstmp);
	return ret;

free_unlock:
	mutex_unlock(&ebt_mutex);
free_iterate:
	EBT_ENTRY_ITERATE(newinfo->entries, newinfo->entries_size,
			  ebt_cleanup_entry, net, NULL);
free_counterstmp:
	vfree(counterstmp);
	/* can be initialized in translate_table() */
	if (newinfo->chainstack) {
		for_each_possible_cpu(i)
			vfree(newinfo->chainstack[i]);
		vfree(newinfo->chainstack);
	}
	return ret;
}