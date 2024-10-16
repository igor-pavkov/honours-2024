static int btrfs_ioctl_setflags(struct file *file, void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_inode *ip = BTRFS_I(inode);
	struct btrfs_root *root = ip->root;
	struct btrfs_trans_handle *trans;
	unsigned int flags, oldflags;
	int ret;
	u64 ip_oldflags;
	unsigned int i_oldflags;
	umode_t mode;

	if (!inode_owner_or_capable(inode))
		return -EPERM;

	if (btrfs_root_readonly(root))
		return -EROFS;

	if (copy_from_user(&flags, arg, sizeof(flags)))
		return -EFAULT;

	ret = check_flags(flags);
	if (ret)
		return ret;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	mutex_lock(&inode->i_mutex);

	ip_oldflags = ip->flags;
	i_oldflags = inode->i_flags;
	mode = inode->i_mode;

	flags = btrfs_mask_flags(inode->i_mode, flags);
	oldflags = btrfs_flags_to_ioctl(ip->flags);
	if ((flags ^ oldflags) & (FS_APPEND_FL | FS_IMMUTABLE_FL)) {
		if (!capable(CAP_LINUX_IMMUTABLE)) {
			ret = -EPERM;
			goto out_unlock;
		}
	}

	if (flags & FS_SYNC_FL)
		ip->flags |= BTRFS_INODE_SYNC;
	else
		ip->flags &= ~BTRFS_INODE_SYNC;
	if (flags & FS_IMMUTABLE_FL)
		ip->flags |= BTRFS_INODE_IMMUTABLE;
	else
		ip->flags &= ~BTRFS_INODE_IMMUTABLE;
	if (flags & FS_APPEND_FL)
		ip->flags |= BTRFS_INODE_APPEND;
	else
		ip->flags &= ~BTRFS_INODE_APPEND;
	if (flags & FS_NODUMP_FL)
		ip->flags |= BTRFS_INODE_NODUMP;
	else
		ip->flags &= ~BTRFS_INODE_NODUMP;
	if (flags & FS_NOATIME_FL)
		ip->flags |= BTRFS_INODE_NOATIME;
	else
		ip->flags &= ~BTRFS_INODE_NOATIME;
	if (flags & FS_DIRSYNC_FL)
		ip->flags |= BTRFS_INODE_DIRSYNC;
	else
		ip->flags &= ~BTRFS_INODE_DIRSYNC;
	if (flags & FS_NOCOW_FL) {
		if (S_ISREG(mode)) {
			/*
			 * It's safe to turn csums off here, no extents exist.
			 * Otherwise we want the flag to reflect the real COW
			 * status of the file and will not set it.
			 */
			if (inode->i_size == 0)
				ip->flags |= BTRFS_INODE_NODATACOW
					   | BTRFS_INODE_NODATASUM;
		} else {
			ip->flags |= BTRFS_INODE_NODATACOW;
		}
	} else {
		/*
		 * Revert back under same assuptions as above
		 */
		if (S_ISREG(mode)) {
			if (inode->i_size == 0)
				ip->flags &= ~(BTRFS_INODE_NODATACOW
				             | BTRFS_INODE_NODATASUM);
		} else {
			ip->flags &= ~BTRFS_INODE_NODATACOW;
		}
	}

	/*
	 * The COMPRESS flag can only be changed by users, while the NOCOMPRESS
	 * flag may be changed automatically if compression code won't make
	 * things smaller.
	 */
	if (flags & FS_NOCOMP_FL) {
		ip->flags &= ~BTRFS_INODE_COMPRESS;
		ip->flags |= BTRFS_INODE_NOCOMPRESS;

		ret = btrfs_set_prop(inode, "btrfs.compression", NULL, 0, 0);
		if (ret && ret != -ENODATA)
			goto out_drop;
	} else if (flags & FS_COMPR_FL) {
		const char *comp;

		ip->flags |= BTRFS_INODE_COMPRESS;
		ip->flags &= ~BTRFS_INODE_NOCOMPRESS;

		if (root->fs_info->compress_type == BTRFS_COMPRESS_LZO)
			comp = "lzo";
		else
			comp = "zlib";
		ret = btrfs_set_prop(inode, "btrfs.compression",
				     comp, strlen(comp), 0);
		if (ret)
			goto out_drop;

	} else {
		ret = btrfs_set_prop(inode, "btrfs.compression", NULL, 0, 0);
		if (ret && ret != -ENODATA)
			goto out_drop;
		ip->flags &= ~(BTRFS_INODE_COMPRESS | BTRFS_INODE_NOCOMPRESS);
	}

	trans = btrfs_start_transaction(root, 1);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto out_drop;
	}

	btrfs_update_iflags(inode);
	inode_inc_iversion(inode);
	inode->i_ctime = CURRENT_TIME;
	ret = btrfs_update_inode(trans, root, inode);

	btrfs_end_transaction(trans, root);
 out_drop:
	if (ret) {
		ip->flags = ip_oldflags;
		inode->i_flags = i_oldflags;
	}

 out_unlock:
	mutex_unlock(&inode->i_mutex);
	mnt_drop_write_file(file);
	return ret;
}