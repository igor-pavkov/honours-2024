static int graft_tree(struct vfsmount *mnt, struct nameidata *nd)
{
	int err;
	if (mnt->mnt_sb->s_flags & MS_NOUSER)
		return -EINVAL;

	if (S_ISDIR(nd->dentry->d_inode->i_mode) !=
	      S_ISDIR(mnt->mnt_root->d_inode->i_mode))
		return -ENOTDIR;

	err = -ENOENT;
	mutex_lock(&nd->dentry->d_inode->i_mutex);
	if (IS_DEADDIR(nd->dentry->d_inode))
		goto out_unlock;

	err = security_sb_check_sb(mnt, nd);
	if (err)
		goto out_unlock;

	err = -ENOENT;
	if (IS_ROOT(nd->dentry) || !d_unhashed(nd->dentry))
		err = attach_recursive_mnt(mnt, nd, NULL);
out_unlock:
	mutex_unlock(&nd->dentry->d_inode->i_mutex);
	if (!err)
		security_sb_post_addmount(mnt, nd);
	return err;
}