int f2fs_preallocate_blocks(struct kiocb *iocb, struct iov_iter *from)
{
	struct inode *inode = file_inode(iocb->ki_filp);
	struct f2fs_map_blocks map;
	int err = 0;

	if (is_inode_flag_set(inode, FI_NO_PREALLOC))
		return 0;

	map.m_lblk = F2FS_BLK_ALIGN(iocb->ki_pos);
	map.m_len = F2FS_BYTES_TO_BLK(iocb->ki_pos + iov_iter_count(from));
	if (map.m_len > map.m_lblk)
		map.m_len -= map.m_lblk;
	else
		map.m_len = 0;

	map.m_next_pgofs = NULL;

	if (iocb->ki_flags & IOCB_DIRECT) {
		err = f2fs_convert_inline_inode(inode);
		if (err)
			return err;
		return f2fs_map_blocks(inode, &map, 1,
			__force_buffered_io(inode, WRITE) ?
				F2FS_GET_BLOCK_PRE_AIO :
				F2FS_GET_BLOCK_PRE_DIO);
	}
	if (iocb->ki_pos + iov_iter_count(from) > MAX_INLINE_DATA) {
		err = f2fs_convert_inline_inode(inode);
		if (err)
			return err;
	}
	if (!f2fs_has_inline_data(inode))
		return f2fs_map_blocks(inode, &map, 1, F2FS_GET_BLOCK_PRE_AIO);
	return err;
}