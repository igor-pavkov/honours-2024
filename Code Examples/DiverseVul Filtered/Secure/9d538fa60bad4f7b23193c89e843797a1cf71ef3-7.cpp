static int groups_to_user(gid_t __user *dst, const struct group_info *src)
{
	struct user_namespace *user_ns = current_user_ns();
	int i;

	for (i = 0; i < src->ngroups; i++)
		if (put_user(from_kgid_munged(user_ns, src->gid[i]), dst + i))
			return -EFAULT;

	return 0;
}