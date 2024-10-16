static int compat_target_to_user(struct ebt_entry_target *t,
				 void __user **dstptr,
				 unsigned int *size)
{
	const struct xt_target *target = t->u.target;
	struct compat_ebt_entry_mwt __user *cm = *dstptr;
	int off = xt_compat_target_offset(target);
	compat_uint_t tsize = t->target_size - off;

	BUG_ON(off >= t->target_size);

	if (copy_to_user(cm->u.name, target->name,
	    strlen(target->name) + 1) || put_user(tsize, &cm->match_size))
		return -EFAULT;

	if (target->compat_to_user) {
		if (target->compat_to_user(cm->data, t->data))
			return -EFAULT;
	} else if (copy_to_user(cm->data, t->data, tsize))
		return -EFAULT;

	*size -= ebt_compat_entry_padsize() + off;
	*dstptr = cm->data;
	*dstptr += tsize;
	return 0;
}