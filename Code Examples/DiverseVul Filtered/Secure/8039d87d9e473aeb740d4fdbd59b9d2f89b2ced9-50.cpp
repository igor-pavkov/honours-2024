static long btrfs_ioctl_file_extent_same(struct file *file,
			struct btrfs_ioctl_same_args __user *argp)
{
	struct btrfs_ioctl_same_args *same = NULL;
	struct btrfs_ioctl_same_extent_info *info;
	struct inode *src = file_inode(file);
	u64 off;
	u64 len;
	int i;
	int ret;
	unsigned long size;
	u64 bs = BTRFS_I(src)->root->fs_info->sb->s_blocksize;
	bool is_admin = capable(CAP_SYS_ADMIN);
	u16 count;

	if (!(file->f_mode & FMODE_READ))
		return -EINVAL;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	if (get_user(count, &argp->dest_count)) {
		ret = -EFAULT;
		goto out;
	}

	size = offsetof(struct btrfs_ioctl_same_args __user, info[count]);

	same = memdup_user(argp, size);

	if (IS_ERR(same)) {
		ret = PTR_ERR(same);
		same = NULL;
		goto out;
	}

	off = same->logical_offset;
	len = same->length;

	/*
	 * Limit the total length we will dedupe for each operation.
	 * This is intended to bound the total time spent in this
	 * ioctl to something sane.
	 */
	if (len > BTRFS_MAX_DEDUPE_LEN)
		len = BTRFS_MAX_DEDUPE_LEN;

	if (WARN_ON_ONCE(bs < PAGE_CACHE_SIZE)) {
		/*
		 * Btrfs does not support blocksize < page_size. As a
		 * result, btrfs_cmp_data() won't correctly handle
		 * this situation without an update.
		 */
		ret = -EINVAL;
		goto out;
	}

	ret = -EISDIR;
	if (S_ISDIR(src->i_mode))
		goto out;

	ret = -EACCES;
	if (!S_ISREG(src->i_mode))
		goto out;

	/* pre-format output fields to sane values */
	for (i = 0; i < count; i++) {
		same->info[i].bytes_deduped = 0ULL;
		same->info[i].status = 0;
	}

	for (i = 0, info = same->info; i < count; i++, info++) {
		struct inode *dst;
		struct fd dst_file = fdget(info->fd);
		if (!dst_file.file) {
			info->status = -EBADF;
			continue;
		}
		dst = file_inode(dst_file.file);

		if (!(is_admin || (dst_file.file->f_mode & FMODE_WRITE))) {
			info->status = -EINVAL;
		} else if (file->f_path.mnt != dst_file.file->f_path.mnt) {
			info->status = -EXDEV;
		} else if (S_ISDIR(dst->i_mode)) {
			info->status = -EISDIR;
		} else if (!S_ISREG(dst->i_mode)) {
			info->status = -EACCES;
		} else {
			info->status = btrfs_extent_same(src, off, len, dst,
							info->logical_offset);
			if (info->status == 0)
				info->bytes_deduped += len;
		}
		fdput(dst_file);
	}

	ret = copy_to_user(argp, same, size);
	if (ret)
		ret = -EFAULT;

out:
	mnt_drop_write_file(file);
	kfree(same);
	return ret;
}