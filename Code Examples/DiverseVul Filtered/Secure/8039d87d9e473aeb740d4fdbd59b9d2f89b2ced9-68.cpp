static int btrfs_extent_same(struct inode *src, u64 loff, u64 olen,
			     struct inode *dst, u64 dst_loff)
{
	int ret;
	u64 len = olen;
	struct cmp_pages cmp;
	int same_inode = 0;
	u64 same_lock_start = 0;
	u64 same_lock_len = 0;

	if (src == dst)
		same_inode = 1;

	if (len == 0)
		return 0;

	if (same_inode) {
		mutex_lock(&src->i_mutex);

		ret = extent_same_check_offsets(src, loff, &len, olen);
		if (ret)
			goto out_unlock;

		/*
		 * Single inode case wants the same checks, except we
		 * don't want our length pushed out past i_size as
		 * comparing that data range makes no sense.
		 *
		 * extent_same_check_offsets() will do this for an
		 * unaligned length at i_size, so catch it here and
		 * reject the request.
		 *
		 * This effectively means we require aligned extents
		 * for the single-inode case, whereas the other cases
		 * allow an unaligned length so long as it ends at
		 * i_size.
		 */
		if (len != olen) {
			ret = -EINVAL;
			goto out_unlock;
		}

		/* Check for overlapping ranges */
		if (dst_loff + len > loff && dst_loff < loff + len) {
			ret = -EINVAL;
			goto out_unlock;
		}

		same_lock_start = min_t(u64, loff, dst_loff);
		same_lock_len = max_t(u64, loff, dst_loff) + len - same_lock_start;
	} else {
		btrfs_double_inode_lock(src, dst);

		ret = extent_same_check_offsets(src, loff, &len, olen);
		if (ret)
			goto out_unlock;

		ret = extent_same_check_offsets(dst, dst_loff, &len, olen);
		if (ret)
			goto out_unlock;
	}

	/* don't make the dst file partly checksummed */
	if ((BTRFS_I(src)->flags & BTRFS_INODE_NODATASUM) !=
	    (BTRFS_I(dst)->flags & BTRFS_INODE_NODATASUM)) {
		ret = -EINVAL;
		goto out_unlock;
	}

	ret = btrfs_cmp_data_prepare(src, loff, dst, dst_loff, olen, &cmp);
	if (ret)
		goto out_unlock;

	if (same_inode)
		lock_extent_range(src, same_lock_start, same_lock_len);
	else
		btrfs_double_extent_lock(src, loff, dst, dst_loff, len);

	/* pass original length for comparison so we stay within i_size */
	ret = btrfs_cmp_data(src, loff, dst, dst_loff, olen, &cmp);
	if (ret == 0)
		ret = btrfs_clone(src, dst, loff, olen, len, dst_loff, 1);

	if (same_inode)
		unlock_extent(&BTRFS_I(src)->io_tree, same_lock_start,
			      same_lock_start + same_lock_len - 1);
	else
		btrfs_double_extent_unlock(src, loff, dst, dst_loff, len);

	btrfs_cmp_data_free(&cmp);
out_unlock:
	if (same_inode)
		mutex_unlock(&src->i_mutex);
	else
		btrfs_double_inode_unlock(src, dst);

	return ret;
}