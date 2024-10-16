void ext4_error(struct super_block *sb, const char *function,
		const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	printk(KERN_CRIT "EXT4-fs error (device %s): %s: ", sb->s_id, function);
	vprintk(fmt, args);
	printk("\n");
	va_end(args);

	ext4_handle_error(sb);
}