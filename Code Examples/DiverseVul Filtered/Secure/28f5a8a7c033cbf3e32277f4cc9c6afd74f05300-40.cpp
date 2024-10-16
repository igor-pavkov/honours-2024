static loff_t ocfs2_file_llseek(struct file *file, loff_t offset, int whence)
{
	struct inode *inode = file->f_mapping->host;
	int ret = 0;

	inode_lock(inode);

	switch (whence) {
	case SEEK_SET:
		break;
	case SEEK_END:
		/* SEEK_END requires the OCFS2 inode lock for the file
		 * because it references the file's size.
		 */
		ret = ocfs2_inode_lock(inode, NULL, 0);
		if (ret < 0) {
			mlog_errno(ret);
			goto out;
		}
		offset += i_size_read(inode);
		ocfs2_inode_unlock(inode, 0);
		break;
	case SEEK_CUR:
		if (offset == 0) {
			offset = file->f_pos;
			goto out;
		}
		offset += file->f_pos;
		break;
	case SEEK_DATA:
	case SEEK_HOLE:
		ret = ocfs2_seek_data_hole_offset(file, &offset, whence);
		if (ret)
			goto out;
		break;
	default:
		ret = -EINVAL;
		goto out;
	}

	offset = vfs_setpos(file, offset, inode->i_sb->s_maxbytes);

out:
	inode_unlock(inode);
	if (ret)
		return ret;
	return offset;
}