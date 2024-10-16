static int compat_match_to_user(struct ebt_entry_match *m, void __user **dstptr,
				unsigned int *size)
{
	const struct xt_match *match = m->u.match;
	struct compat_ebt_entry_mwt __user *cm = *dstptr;
	int off = ebt_compat_match_offset(match, m->match_size);
	compat_uint_t msize = m->match_size - off;

	BUG_ON(off >= m->match_size);

	if (copy_to_user(cm->u.name, match->name,
	    strlen(match->name) + 1) || put_user(msize, &cm->match_size))
		return -EFAULT;

	if (match->compat_to_user) {
		if (match->compat_to_user(cm->data, m->data))
			return -EFAULT;
	} else if (copy_to_user(cm->data, m->data, msize))
			return -EFAULT;

	*size -= ebt_compat_entry_padsize() + off;
	*dstptr = cm->data;
	*dstptr += msize;
	return 0;
}