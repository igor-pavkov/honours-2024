static int minix_statfs(struct dentry *dentry, struct kstatfs *buf)
{
	struct minix_sb_info *sbi = minix_sb(dentry->d_sb);
	buf->f_type = dentry->d_sb->s_magic;
	buf->f_bsize = dentry->d_sb->s_blocksize;
	buf->f_blocks = (sbi->s_nzones - sbi->s_firstdatazone) << sbi->s_log_zone_size;
	buf->f_bfree = minix_count_free_blocks(sbi);
	buf->f_bavail = buf->f_bfree;
	buf->f_files = sbi->s_ninodes;
	buf->f_ffree = minix_count_free_inodes(sbi);
	buf->f_namelen = sbi->s_namelen;
	return 0;
}