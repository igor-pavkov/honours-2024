static int ext4_statfs(struct dentry *dentry, struct kstatfs *buf)
{
	struct super_block *sb = dentry->d_sb;
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_super_block *es = sbi->s_es;
	ext4_fsblk_t overhead = 0, resv_blocks;
	u64 fsid;
	s64 bfree;
	resv_blocks = EXT4_C2B(sbi, atomic64_read(&sbi->s_resv_clusters));

	if (!test_opt(sb, MINIX_DF))
		overhead = sbi->s_overhead;

	buf->f_type = EXT4_SUPER_MAGIC;
	buf->f_bsize = sb->s_blocksize;
	buf->f_blocks = ext4_blocks_count(es) - EXT4_C2B(sbi, overhead);
	bfree = percpu_counter_sum_positive(&sbi->s_freeclusters_counter) -
		percpu_counter_sum_positive(&sbi->s_dirtyclusters_counter);
	/* prevent underflow in case that few free space is available */
	buf->f_bfree = EXT4_C2B(sbi, max_t(s64, bfree, 0));
	buf->f_bavail = buf->f_bfree -
			(ext4_r_blocks_count(es) + resv_blocks);
	if (buf->f_bfree < (ext4_r_blocks_count(es) + resv_blocks))
		buf->f_bavail = 0;
	buf->f_files = le32_to_cpu(es->s_inodes_count);
	buf->f_ffree = percpu_counter_sum_positive(&sbi->s_freeinodes_counter);
	buf->f_namelen = EXT4_NAME_LEN;
	fsid = le64_to_cpup((void *)es->s_uuid) ^
	       le64_to_cpup((void *)es->s_uuid + sizeof(u64));
	buf->f_fsid.val[0] = fsid & 0xFFFFFFFFUL;
	buf->f_fsid.val[1] = (fsid >> 32) & 0xFFFFFFFFUL;

#ifdef CONFIG_QUOTA
	if (ext4_test_inode_flag(dentry->d_inode, EXT4_INODE_PROJINHERIT) &&
	    sb_has_quota_limits_enabled(sb, PRJQUOTA))
		ext4_statfs_project(sb, EXT4_I(dentry->d_inode)->i_projid, buf);
#endif
	return 0;
}