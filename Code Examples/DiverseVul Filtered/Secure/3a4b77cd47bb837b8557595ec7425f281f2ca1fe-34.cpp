static void print_daily_error_info(unsigned long arg)
{
	struct super_block *sb = (struct super_block *) arg;
	struct ext4_sb_info *sbi;
	struct ext4_super_block *es;

	sbi = EXT4_SB(sb);
	es = sbi->s_es;

	if (es->s_error_count)
		/* fsck newer than v1.41.13 is needed to clean this condition. */
		ext4_msg(sb, KERN_NOTICE, "error count since last fsck: %u",
			 le32_to_cpu(es->s_error_count));
	if (es->s_first_error_time) {
		printk(KERN_NOTICE "EXT4-fs (%s): initial error at time %u: %.*s:%d",
		       sb->s_id, le32_to_cpu(es->s_first_error_time),
		       (int) sizeof(es->s_first_error_func),
		       es->s_first_error_func,
		       le32_to_cpu(es->s_first_error_line));
		if (es->s_first_error_ino)
			printk(KERN_CONT ": inode %u",
			       le32_to_cpu(es->s_first_error_ino));
		if (es->s_first_error_block)
			printk(KERN_CONT ": block %llu", (unsigned long long)
			       le64_to_cpu(es->s_first_error_block));
		printk(KERN_CONT "\n");
	}
	if (es->s_last_error_time) {
		printk(KERN_NOTICE "EXT4-fs (%s): last error at time %u: %.*s:%d",
		       sb->s_id, le32_to_cpu(es->s_last_error_time),
		       (int) sizeof(es->s_last_error_func),
		       es->s_last_error_func,
		       le32_to_cpu(es->s_last_error_line));
		if (es->s_last_error_ino)
			printk(KERN_CONT ": inode %u",
			       le32_to_cpu(es->s_last_error_ino));
		if (es->s_last_error_block)
			printk(KERN_CONT ": block %llu", (unsigned long long)
			       le64_to_cpu(es->s_last_error_block));
		printk(KERN_CONT "\n");
	}
	mod_timer(&sbi->s_err_report, jiffies + 24*60*60*HZ);  /* Once a day */
}