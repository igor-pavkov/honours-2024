int do_add_mount(struct vfsmount *newmnt, struct nameidata *nd,
		 int mnt_flags, struct list_head *fslist)
{
	int err;

	down_write(&namespace_sem);
	/* Something was mounted here while we slept */
	while (d_mountpoint(nd->dentry) && follow_down(&nd->mnt, &nd->dentry))
		;
	err = -EINVAL;
	if (!check_mnt(nd->mnt))
		goto unlock;

	/* Refuse the same filesystem on the same mount point */
	err = -EBUSY;
	if (nd->mnt->mnt_sb == newmnt->mnt_sb &&
	    nd->mnt->mnt_root == nd->dentry)
		goto unlock;

	err = -EINVAL;
	if (S_ISLNK(newmnt->mnt_root->d_inode->i_mode))
		goto unlock;

	newmnt->mnt_flags = mnt_flags;
	if ((err = graft_tree(newmnt, nd)))
		goto unlock;

	if (fslist) {
		/* add to the specified expiration list */
		spin_lock(&vfsmount_lock);
		list_add_tail(&newmnt->mnt_expire, fslist);
		spin_unlock(&vfsmount_lock);
	}
	up_write(&namespace_sem);
	return 0;

unlock:
	up_write(&namespace_sem);
	mntput(newmnt);
	return err;
}