static int ocfs2_prepare_inode_for_write(struct file *file,
					 loff_t pos,
					 size_t count)
{
	int ret = 0, meta_level = 0;
	struct dentry *dentry = file->f_path.dentry;
	struct inode *inode = d_inode(dentry);
	loff_t end;

	/*
	 * We start with a read level meta lock and only jump to an ex
	 * if we need to make modifications here.
	 */
	for(;;) {
		ret = ocfs2_inode_lock(inode, NULL, meta_level);
		if (ret < 0) {
			meta_level = -1;
			mlog_errno(ret);
			goto out;
		}

		/* Clear suid / sgid if necessary. We do this here
		 * instead of later in the write path because
		 * remove_suid() calls ->setattr without any hint that
		 * we may have already done our cluster locking. Since
		 * ocfs2_setattr() *must* take cluster locks to
		 * proceed, this will lead us to recursively lock the
		 * inode. There's also the dinode i_size state which
		 * can be lost via setattr during extending writes (we
		 * set inode->i_size at the end of a write. */
		if (should_remove_suid(dentry)) {
			if (meta_level == 0) {
				ocfs2_inode_unlock(inode, meta_level);
				meta_level = 1;
				continue;
			}

			ret = ocfs2_write_remove_suid(inode);
			if (ret < 0) {
				mlog_errno(ret);
				goto out_unlock;
			}
		}

		end = pos + count;

		ret = ocfs2_check_range_for_refcount(inode, pos, count);
		if (ret == 1) {
			ocfs2_inode_unlock(inode, meta_level);
			meta_level = -1;

			ret = ocfs2_prepare_inode_for_refcount(inode,
							       file,
							       pos,
							       count,
							       &meta_level);
		}

		if (ret < 0) {
			mlog_errno(ret);
			goto out_unlock;
		}

		break;
	}

out_unlock:
	trace_ocfs2_prepare_inode_for_write(OCFS2_I(inode)->ip_blkno,
					    pos, count);

	if (meta_level >= 0)
		ocfs2_inode_unlock(inode, meta_level);

out:
	return ret;
}