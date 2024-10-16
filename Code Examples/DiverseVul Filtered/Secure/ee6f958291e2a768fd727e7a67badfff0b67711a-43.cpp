static int do_remount(struct nameidata *nd, int flags, int mnt_flags,
		      void *data)
{
	int err;
	struct super_block *sb = nd->mnt->mnt_sb;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (!check_mnt(nd->mnt))
		return -EINVAL;

	if (nd->dentry != nd->mnt->mnt_root)
		return -EINVAL;

	down_write(&sb->s_umount);
	err = do_remount_sb(sb, flags, data, 0);
	if (!err)
		nd->mnt->mnt_flags = mnt_flags;
	up_write(&sb->s_umount);
	if (!err)
		security_sb_post_remount(nd->mnt, flags, data);
	return err;
}