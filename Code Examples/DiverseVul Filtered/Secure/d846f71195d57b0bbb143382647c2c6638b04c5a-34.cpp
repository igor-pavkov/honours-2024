static int compat_do_replace(struct net *net, void __user *user,
			     unsigned int len)
{
	int ret, i, countersize, size64;
	struct ebt_table_info *newinfo;
	struct ebt_replace tmp;
	struct ebt_entries_buf_state state;
	void *entries_tmp;

	ret = compat_copy_ebt_replace_from_user(&tmp, user, len);
	if (ret) {
		/* try real handler in case userland supplied needed padding */
		if (ret == -EINVAL && do_replace(net, user, len) == 0)
			ret = 0;
		return ret;
	}

	countersize = COUNTER_OFFSET(tmp.nentries) * nr_cpu_ids;
	newinfo = vmalloc(sizeof(*newinfo) + countersize);
	if (!newinfo)
		return -ENOMEM;

	if (countersize)
		memset(newinfo->counters, 0, countersize);

	memset(&state, 0, sizeof(state));

	newinfo->entries = vmalloc(tmp.entries_size);
	if (!newinfo->entries) {
		ret = -ENOMEM;
		goto free_newinfo;
	}
	if (copy_from_user(
	   newinfo->entries, tmp.entries, tmp.entries_size) != 0) {
		ret = -EFAULT;
		goto free_entries;
	}

	entries_tmp = newinfo->entries;

	xt_compat_lock(NFPROTO_BRIDGE);

	ret = compat_copy_entries(entries_tmp, tmp.entries_size, &state);
	if (ret < 0)
		goto out_unlock;

	pr_debug("tmp.entries_size %d, kern off %d, user off %d delta %d\n",
		tmp.entries_size, state.buf_kern_offset, state.buf_user_offset,
		xt_compat_calc_jump(NFPROTO_BRIDGE, tmp.entries_size));

	size64 = ret;
	newinfo->entries = vmalloc(size64);
	if (!newinfo->entries) {
		vfree(entries_tmp);
		ret = -ENOMEM;
		goto out_unlock;
	}

	memset(&state, 0, sizeof(state));
	state.buf_kern_start = newinfo->entries;
	state.buf_kern_len = size64;

	ret = compat_copy_entries(entries_tmp, tmp.entries_size, &state);
	BUG_ON(ret < 0);	/* parses same data again */

	vfree(entries_tmp);
	tmp.entries_size = size64;

	for (i = 0; i < NF_BR_NUMHOOKS; i++) {
		char __user *usrptr;
		if (tmp.hook_entry[i]) {
			unsigned int delta;
			usrptr = (char __user *) tmp.hook_entry[i];
			delta = usrptr - tmp.entries;
			usrptr += xt_compat_calc_jump(NFPROTO_BRIDGE, delta);
			tmp.hook_entry[i] = (struct ebt_entries __user *)usrptr;
		}
	}

	xt_compat_flush_offsets(NFPROTO_BRIDGE);
	xt_compat_unlock(NFPROTO_BRIDGE);

	ret = do_replace_finish(net, &tmp, newinfo);
	if (ret == 0)
		return ret;
free_entries:
	vfree(newinfo->entries);
free_newinfo:
	vfree(newinfo);
	return ret;
out_unlock:
	xt_compat_flush_offsets(NFPROTO_BRIDGE);
	xt_compat_unlock(NFPROTO_BRIDGE);
	goto free_entries;
}