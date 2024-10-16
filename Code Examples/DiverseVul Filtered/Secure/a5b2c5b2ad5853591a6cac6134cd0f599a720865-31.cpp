static int apparmor_path_truncate(struct path *path)
{
	struct path_cond cond = { path->dentry->d_inode->i_uid,
				  path->dentry->d_inode->i_mode
	};

	if (!path->mnt || !mediated_filesystem(path->dentry->d_inode))
		return 0;

	return common_perm(OP_TRUNC, path, MAY_WRITE | AA_MAY_META_WRITE,
			   &cond);
}