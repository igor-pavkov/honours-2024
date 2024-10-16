void mnt_pin(struct vfsmount *mnt)
{
	lock_mount_hash();
	real_mount(mnt)->mnt_pinned++;
	unlock_mount_hash();
}