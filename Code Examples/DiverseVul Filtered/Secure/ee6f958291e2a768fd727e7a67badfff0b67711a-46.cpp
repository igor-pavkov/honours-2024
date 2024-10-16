int may_umount(struct vfsmount *mnt)
{
	int ret = 1;
	spin_lock(&vfsmount_lock);
	if (propagate_mount_busy(mnt, 2))
		ret = 0;
	spin_unlock(&vfsmount_lock);
	return ret;
}