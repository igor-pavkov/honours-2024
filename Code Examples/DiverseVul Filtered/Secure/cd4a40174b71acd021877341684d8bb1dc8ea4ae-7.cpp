struct vfsmount *clone_private_mount(struct path *path)
{
	struct mount *old_mnt = real_mount(path->mnt);
	struct mount *new_mnt;

	if (IS_MNT_UNBINDABLE(old_mnt))
		return ERR_PTR(-EINVAL);

	down_read(&namespace_sem);
	new_mnt = clone_mnt(old_mnt, path->dentry, CL_PRIVATE);
	up_read(&namespace_sem);
	if (IS_ERR(new_mnt))
		return ERR_CAST(new_mnt);

	return &new_mnt->mnt;
}