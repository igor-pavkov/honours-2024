void inode_init_owner(struct inode *inode, const struct inode *dir,
			umode_t mode)
{
	inode->i_uid = current_fsuid();
	if (dir && dir->i_mode & S_ISGID) {
		inode->i_gid = dir->i_gid;

		/* Directories are special, and always inherit S_ISGID */
		if (S_ISDIR(mode))
			mode |= S_ISGID;
		else if ((mode & (S_ISGID | S_IXGRP)) == (S_ISGID | S_IXGRP) &&
			 !in_group_p(inode->i_gid) &&
			 !capable_wrt_inode_uidgid(dir, CAP_FSETID))
			mode &= ~S_ISGID;
	} else
		inode->i_gid = current_fsgid();
	inode->i_mode = mode;
}