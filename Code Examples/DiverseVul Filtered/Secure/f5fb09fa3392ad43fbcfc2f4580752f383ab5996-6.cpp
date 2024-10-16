static void V1_minix_read_inode(struct inode * inode)
{
	struct buffer_head * bh;
	struct minix_inode * raw_inode;
	struct minix_inode_info *minix_inode = minix_i(inode);
	int i;

	raw_inode = minix_V1_raw_inode(inode->i_sb, inode->i_ino, &bh);
	if (!raw_inode) {
		make_bad_inode(inode);
		return;
	}
	inode->i_mode = raw_inode->i_mode;
	inode->i_uid = (uid_t)raw_inode->i_uid;
	inode->i_gid = (gid_t)raw_inode->i_gid;
	inode->i_nlink = raw_inode->i_nlinks;
	inode->i_size = raw_inode->i_size;
	inode->i_mtime.tv_sec = inode->i_atime.tv_sec = inode->i_ctime.tv_sec = raw_inode->i_time;
	inode->i_mtime.tv_nsec = 0;
	inode->i_atime.tv_nsec = 0;
	inode->i_ctime.tv_nsec = 0;
	inode->i_blocks = inode->i_blksize = 0;
	for (i = 0; i < 9; i++)
		minix_inode->u.i1_data[i] = raw_inode->i_zone[i];
	minix_set_inode(inode, old_decode_dev(raw_inode->i_zone[0]));
	brelse(bh);
}