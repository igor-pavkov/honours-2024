static int do_move_mount(struct nameidata *nd, char *old_name)
{
	struct nameidata old_nd, parent_nd;
	struct vfsmount *p;
	int err = 0;
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	if (!old_name || !*old_name)
		return -EINVAL;
	err = path_lookup(old_name, LOOKUP_FOLLOW, &old_nd);
	if (err)
		return err;

	down_write(&namespace_sem);
	while (d_mountpoint(nd->dentry) && follow_down(&nd->mnt, &nd->dentry))
		;
	err = -EINVAL;
	if (!check_mnt(nd->mnt) || !check_mnt(old_nd.mnt))
		goto out;

	err = -ENOENT;
	mutex_lock(&nd->dentry->d_inode->i_mutex);
	if (IS_DEADDIR(nd->dentry->d_inode))
		goto out1;

	if (!IS_ROOT(nd->dentry) && d_unhashed(nd->dentry))
		goto out1;

	err = -EINVAL;
	if (old_nd.dentry != old_nd.mnt->mnt_root)
		goto out1;

	if (old_nd.mnt == old_nd.mnt->mnt_parent)
		goto out1;

	if (S_ISDIR(nd->dentry->d_inode->i_mode) !=
	      S_ISDIR(old_nd.dentry->d_inode->i_mode))
		goto out1;
	/*
	 * Don't move a mount residing in a shared parent.
	 */
	if (old_nd.mnt->mnt_parent && IS_MNT_SHARED(old_nd.mnt->mnt_parent))
		goto out1;
	/*
	 * Don't move a mount tree containing unbindable mounts to a destination
	 * mount which is shared.
	 */
	if (IS_MNT_SHARED(nd->mnt) && tree_contains_unbindable(old_nd.mnt))
		goto out1;
	err = -ELOOP;
	for (p = nd->mnt; p->mnt_parent != p; p = p->mnt_parent)
		if (p == old_nd.mnt)
			goto out1;

	if ((err = attach_recursive_mnt(old_nd.mnt, nd, &parent_nd)))
		goto out1;

	spin_lock(&vfsmount_lock);
	/* if the mount is moved, it should no longer be expire
	 * automatically */
	list_del_init(&old_nd.mnt->mnt_expire);
	spin_unlock(&vfsmount_lock);
out1:
	mutex_unlock(&nd->dentry->d_inode->i_mutex);
out:
	up_write(&namespace_sem);
	if (!err)
		path_release(&parent_nd);
	path_release(&old_nd);
	return err;
}