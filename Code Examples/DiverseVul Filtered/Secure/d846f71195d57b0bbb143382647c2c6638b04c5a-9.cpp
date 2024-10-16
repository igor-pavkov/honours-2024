static int copy_everything_to_user(struct ebt_table *t, void __user *user,
    const int *len, int cmd)
{
	struct ebt_replace tmp;
	const struct ebt_counter *oldcounters;
	unsigned int entries_size, nentries;
	int ret;
	char *entries;

	if (cmd == EBT_SO_GET_ENTRIES) {
		entries_size = t->private->entries_size;
		nentries = t->private->nentries;
		entries = t->private->entries;
		oldcounters = t->private->counters;
	} else {
		entries_size = t->table->entries_size;
		nentries = t->table->nentries;
		entries = t->table->entries;
		oldcounters = t->table->counters;
	}

	if (copy_from_user(&tmp, user, sizeof(tmp)))
		return -EFAULT;

	if (*len != sizeof(struct ebt_replace) + entries_size +
	   (tmp.num_counters? nentries * sizeof(struct ebt_counter): 0))
		return -EINVAL;

	if (tmp.nentries != nentries) {
		BUGPRINT("Nentries wrong\n");
		return -EINVAL;
	}

	if (tmp.entries_size != entries_size) {
		BUGPRINT("Wrong size\n");
		return -EINVAL;
	}

	ret = copy_counters_to_user(t, oldcounters, tmp.counters,
					tmp.num_counters, nentries);
	if (ret)
		return ret;

	if (copy_to_user(tmp.entries, entries, entries_size)) {
		BUGPRINT("Couldn't copy entries to userspace\n");
		return -EFAULT;
	}
	/* set the match/watcher/target names right */
	return EBT_ENTRY_ITERATE(entries, entries_size,
	   ebt_make_names, entries, tmp.entries);
}