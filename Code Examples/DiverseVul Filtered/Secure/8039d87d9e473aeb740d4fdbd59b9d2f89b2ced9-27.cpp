static noinline long btrfs_ioctl_clone(struct file *file, unsigned long srcfd,
				       u64 off, u64 olen, u64 destoff)
{
	struct inode *inode = file_inode(file);
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct fd src_file;
	struct inode *src;
	int ret;
	u64 len = olen;
	u64 bs = root->fs_info->sb->s_blocksize;
	int same_inode = 0;

	/*
	 * TODO:
	 * - split compressed inline extents.  annoying: we need to
	 *   decompress into destination's address_space (the file offset
	 *   may change, so source mapping won't do), then recompress (or
	 *   otherwise reinsert) a subrange.
	 *
	 * - split destination inode's inline extents.  The inline extents can
	 *   be either compressed or non-compressed.
	 */

	/* the destination must be opened for writing */
	if (!(file->f_mode & FMODE_WRITE) || (file->f_flags & O_APPEND))
		return -EINVAL;

	if (btrfs_root_readonly(root))
		return -EROFS;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	src_file = fdget(srcfd);
	if (!src_file.file) {
		ret = -EBADF;
		goto out_drop_write;
	}

	ret = -EXDEV;
	if (src_file.file->f_path.mnt != file->f_path.mnt)
		goto out_fput;

	src = file_inode(src_file.file);

	ret = -EINVAL;
	if (src == inode)
		same_inode = 1;

	/* the src must be open for reading */
	if (!(src_file.file->f_mode & FMODE_READ))
		goto out_fput;

	/* don't make the dst file partly checksummed */
	if ((BTRFS_I(src)->flags & BTRFS_INODE_NODATASUM) !=
	    (BTRFS_I(inode)->flags & BTRFS_INODE_NODATASUM))
		goto out_fput;

	ret = -EISDIR;
	if (S_ISDIR(src->i_mode) || S_ISDIR(inode->i_mode))
		goto out_fput;

	ret = -EXDEV;
	if (src->i_sb != inode->i_sb)
		goto out_fput;

	if (!same_inode) {
		btrfs_double_inode_lock(src, inode);
	} else {
		mutex_lock(&src->i_mutex);
	}

	/* determine range to clone */
	ret = -EINVAL;
	if (off + len > src->i_size || off + len < off)
		goto out_unlock;
	if (len == 0)
		olen = len = src->i_size - off;
	/* if we extend to eof, continue to block boundary */
	if (off + len == src->i_size)
		len = ALIGN(src->i_size, bs) - off;

	if (len == 0) {
		ret = 0;
		goto out_unlock;
	}

	/* verify the end result is block aligned */
	if (!IS_ALIGNED(off, bs) || !IS_ALIGNED(off + len, bs) ||
	    !IS_ALIGNED(destoff, bs))
		goto out_unlock;

	/* verify if ranges are overlapped within the same file */
	if (same_inode) {
		if (destoff + len > off && destoff < off + len)
			goto out_unlock;
	}

	if (destoff > inode->i_size) {
		ret = btrfs_cont_expand(inode, inode->i_size, destoff);
		if (ret)
			goto out_unlock;
	}

	/*
	 * Lock the target range too. Right after we replace the file extent
	 * items in the fs tree (which now point to the cloned data), we might
	 * have a worker replace them with extent items relative to a write
	 * operation that was issued before this clone operation (i.e. confront
	 * with inode.c:btrfs_finish_ordered_io).
	 */
	if (same_inode) {
		u64 lock_start = min_t(u64, off, destoff);
		u64 lock_len = max_t(u64, off, destoff) + len - lock_start;

		lock_extent_range(src, lock_start, lock_len);
	} else {
		btrfs_double_extent_lock(src, off, inode, destoff, len);
	}

	ret = btrfs_clone(src, inode, off, olen, len, destoff, 0);

	if (same_inode) {
		u64 lock_start = min_t(u64, off, destoff);
		u64 lock_end = max_t(u64, off, destoff) + len - 1;

		unlock_extent(&BTRFS_I(src)->io_tree, lock_start, lock_end);
	} else {
		btrfs_double_extent_unlock(src, off, inode, destoff, len);
	}
	/*
	 * Truncate page cache pages so that future reads will see the cloned
	 * data immediately and not the previous data.
	 */
	truncate_inode_pages_range(&inode->i_data, destoff,
				   PAGE_CACHE_ALIGN(destoff + len) - 1);
out_unlock:
	if (!same_inode)
		btrfs_double_inode_unlock(src, inode);
	else
		mutex_unlock(&src->i_mutex);
out_fput:
	fdput(src_file);
out_drop_write:
	mnt_drop_write_file(file);
	return ret;
}