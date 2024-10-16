static inline int ebt_make_matchname(const struct ebt_entry_match *m,
    const char *base, char __user *ubase)
{
	char __user *hlp = ubase + ((char *)m - base);
	if (copy_to_user(hlp, m->u.match->name, EBT_FUNCTION_MAXNAMELEN))
		return -EFAULT;
	return 0;
}