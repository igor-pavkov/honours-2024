static int copy_counters_to_user(struct ebt_table *t,
				  const struct ebt_counter *oldcounters,
				  void __user *user, unsigned int num_counters,
				  unsigned int nentries)
{
	struct ebt_counter *counterstmp;
	int ret = 0;

	/* userspace might not need the counters */
	if (num_counters == 0)
		return 0;

	if (num_counters != nentries) {
		BUGPRINT("Num_counters wrong\n");
		return -EINVAL;
	}

	counterstmp = vmalloc(nentries * sizeof(*counterstmp));
	if (!counterstmp)
		return -ENOMEM;

	write_lock_bh(&t->lock);
	get_counters(oldcounters, counterstmp, nentries);
	write_unlock_bh(&t->lock);

	if (copy_to_user(user, counterstmp,
	   nentries * sizeof(struct ebt_counter)))
		ret = -EFAULT;
	vfree(counterstmp);
	return ret;
}