int iterate_mounts(int (*f)(struct vfsmount *, void *), void *arg,
		   struct vfsmount *root)
{
	struct mount *mnt;
	int res = f(root, arg);
	if (res)
		return res;
	list_for_each_entry(mnt, &real_mount(root)->mnt_list, mnt_list) {
		res = f(&mnt->mnt, arg);
		if (res)
			return res;
	}
	return 0;
}