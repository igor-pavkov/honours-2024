void ext4_abort(struct super_block *sb, const char *function,
		const char *fmt, ...)
{
	va_list args;

	printk(KERN_CRIT "ext4_abort called.\n");

	va_start(args, fmt);
	printk(KERN_CRIT "EXT4-fs error (device %s): %s: ", sb->s_id, function);
	vprintk(fmt, args);
	printk("\n");
	va_end(args);

	if (test_opt(sb, ERRORS_PANIC))
		panic("EXT4-fs panic from previous error\n");

	if (sb->s_flags & MS_RDONLY)
		return;

	printk(KERN_CRIT "Remounting filesystem read-only\n");
	EXT4_SB(sb)->s_mount_state |= EXT4_ERROR_FS;
	sb->s_flags |= MS_RDONLY;
	EXT4_SB(sb)->s_mount_opt |= EXT4_MOUNT_ABORT;
	if (EXT4_SB(sb)->s_journal)
		jbd2_journal_abort(EXT4_SB(sb)->s_journal, -EIO);
}