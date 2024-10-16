static int selinux_is_sblabel_mnt(struct super_block *sb)
{
	struct superblock_security_struct *sbsec = sb->s_security;

	return sbsec->behavior == SECURITY_FS_USE_XATTR ||
		sbsec->behavior == SECURITY_FS_USE_TRANS ||
		sbsec->behavior == SECURITY_FS_USE_TASK ||
		sbsec->behavior == SECURITY_FS_USE_NATIVE ||
		/* Special handling. Genfs but also in-core setxattr handler */
		!strcmp(sb->s_type->name, "sysfs") ||
		!strcmp(sb->s_type->name, "pstore") ||
		!strcmp(sb->s_type->name, "debugfs") ||
		!strcmp(sb->s_type->name, "rootfs");
}