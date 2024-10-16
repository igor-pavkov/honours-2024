static int compat_copy_entry_to_user(struct ebt_entry *e, void __user **dstptr,
				unsigned int *size)
{
	struct ebt_entry_target *t;
	struct ebt_entry __user *ce;
	u32 watchers_offset, target_offset, next_offset;
	compat_uint_t origsize;
	int ret;

	if (e->bitmask == 0) {
		if (*size < sizeof(struct ebt_entries))
			return -EINVAL;
		if (copy_to_user(*dstptr, e, sizeof(struct ebt_entries)))
			return -EFAULT;

		*dstptr += sizeof(struct ebt_entries);
		*size -= sizeof(struct ebt_entries);
		return 0;
	}

	if (*size < sizeof(*ce))
		return -EINVAL;

	ce = (struct ebt_entry __user *)*dstptr;
	if (copy_to_user(ce, e, sizeof(*ce)))
		return -EFAULT;

	origsize = *size;
	*dstptr += sizeof(*ce);

	ret = EBT_MATCH_ITERATE(e, compat_match_to_user, dstptr, size);
	if (ret)
		return ret;
	watchers_offset = e->watchers_offset - (origsize - *size);

	ret = EBT_WATCHER_ITERATE(e, compat_watcher_to_user, dstptr, size);
	if (ret)
		return ret;
	target_offset = e->target_offset - (origsize - *size);

	t = (struct ebt_entry_target *) ((char *) e + e->target_offset);

	ret = compat_target_to_user(t, dstptr, size);
	if (ret)
		return ret;
	next_offset = e->next_offset - (origsize - *size);

	if (put_user(watchers_offset, &ce->watchers_offset) ||
	    put_user(target_offset, &ce->target_offset) ||
	    put_user(next_offset, &ce->next_offset))
		return -EFAULT;

	*size -= sizeof(*ce);
	return 0;
}