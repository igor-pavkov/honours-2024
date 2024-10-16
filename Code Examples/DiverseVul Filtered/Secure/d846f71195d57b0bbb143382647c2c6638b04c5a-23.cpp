static inline int ebt_make_watchername(const struct ebt_entry_watcher *w,
    const char *base, char __user *ubase)
{
	char __user *hlp = ubase + ((char *)w - base);
	if (copy_to_user(hlp , w->u.watcher->name, EBT_FUNCTION_MAXNAMELEN))
		return -EFAULT;
	return 0;
}