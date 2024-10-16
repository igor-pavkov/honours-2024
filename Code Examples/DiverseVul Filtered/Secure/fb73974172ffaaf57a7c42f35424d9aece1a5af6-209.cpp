static void delayed_superblock_init(struct super_block *sb, void *unused)
{
	selinux_set_mnt_opts(sb, NULL, 0, NULL);
}