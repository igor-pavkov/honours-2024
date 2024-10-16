static int selinux_sb_kern_mount(struct super_block *sb, int flags, void *data)
{
	const struct cred *cred = current_cred();
	struct common_audit_data ad;
	int rc;

	rc = superblock_doinit(sb, data);
	if (rc)
		return rc;

	/* Allow all mounts performed by the kernel */
	if (flags & MS_KERNMOUNT)
		return 0;

	ad.type = LSM_AUDIT_DATA_DENTRY;
	ad.u.dentry = sb->s_root;
	return superblock_has_perm(cred, sb, FILESYSTEM__MOUNT, &ad);
}