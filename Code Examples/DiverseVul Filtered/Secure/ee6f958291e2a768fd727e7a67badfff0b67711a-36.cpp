asmlinkage long sys_pivot_root(const char __user * new_root,
			       const char __user * put_old)
{
	struct vfsmount *tmp;
	struct nameidata new_nd, old_nd, parent_nd, root_parent, user_nd;
	int error;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	lock_kernel();

	error = __user_walk(new_root, LOOKUP_FOLLOW | LOOKUP_DIRECTORY,
			    &new_nd);
	if (error)
		goto out0;
	error = -EINVAL;
	if (!check_mnt(new_nd.mnt))
		goto out1;

	error = __user_walk(put_old, LOOKUP_FOLLOW | LOOKUP_DIRECTORY, &old_nd);
	if (error)
		goto out1;

	error = security_sb_pivotroot(&old_nd, &new_nd);
	if (error) {
		path_release(&old_nd);
		goto out1;
	}

	read_lock(&current->fs->lock);
	user_nd.mnt = mntget(current->fs->rootmnt);
	user_nd.dentry = dget(current->fs->root);
	read_unlock(&current->fs->lock);
	down_write(&namespace_sem);
	mutex_lock(&old_nd.dentry->d_inode->i_mutex);
	error = -EINVAL;
	if (IS_MNT_SHARED(old_nd.mnt) ||
		IS_MNT_SHARED(new_nd.mnt->mnt_parent) ||
		IS_MNT_SHARED(user_nd.mnt->mnt_parent))
		goto out2;
	if (!check_mnt(user_nd.mnt))
		goto out2;
	error = -ENOENT;
	if (IS_DEADDIR(new_nd.dentry->d_inode))
		goto out2;
	if (d_unhashed(new_nd.dentry) && !IS_ROOT(new_nd.dentry))
		goto out2;
	if (d_unhashed(old_nd.dentry) && !IS_ROOT(old_nd.dentry))
		goto out2;
	error = -EBUSY;
	if (new_nd.mnt == user_nd.mnt || old_nd.mnt == user_nd.mnt)
		goto out2; /* loop, on the same file system  */
	error = -EINVAL;
	if (user_nd.mnt->mnt_root != user_nd.dentry)
		goto out2; /* not a mountpoint */
	if (user_nd.mnt->mnt_parent == user_nd.mnt)
		goto out2; /* not attached */
	if (new_nd.mnt->mnt_root != new_nd.dentry)
		goto out2; /* not a mountpoint */
	if (new_nd.mnt->mnt_parent == new_nd.mnt)
		goto out2; /* not attached */
	tmp = old_nd.mnt; /* make sure we can reach put_old from new_root */
	spin_lock(&vfsmount_lock);
	if (tmp != new_nd.mnt) {
		for (;;) {
			if (tmp->mnt_parent == tmp)
				goto out3; /* already mounted on put_old */
			if (tmp->mnt_parent == new_nd.mnt)
				break;
			tmp = tmp->mnt_parent;
		}
		if (!is_subdir(tmp->mnt_mountpoint, new_nd.dentry))
			goto out3;
	} else if (!is_subdir(old_nd.dentry, new_nd.dentry))
		goto out3;
	detach_mnt(new_nd.mnt, &parent_nd);
	detach_mnt(user_nd.mnt, &root_parent);
	attach_mnt(user_nd.mnt, &old_nd);     /* mount old root on put_old */
	attach_mnt(new_nd.mnt, &root_parent); /* mount new_root on / */
	touch_mnt_namespace(current->nsproxy->mnt_ns);
	spin_unlock(&vfsmount_lock);
	chroot_fs_refs(&user_nd, &new_nd);
	security_sb_post_pivotroot(&user_nd, &new_nd);
	error = 0;
	path_release(&root_parent);
	path_release(&parent_nd);
out2:
	mutex_unlock(&old_nd.dentry->d_inode->i_mutex);
	up_write(&namespace_sem);
	path_release(&user_nd);
	path_release(&old_nd);
out1:
	path_release(&new_nd);
out0:
	unlock_kernel();
	return error;
out3:
	spin_unlock(&vfsmount_lock);
	goto out2;
}