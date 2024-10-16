void drop_collected_mounts(struct vfsmount *mnt)
{
	namespace_lock();
	lock_mount_hash();
	umount_tree(real_mount(mnt), 0);
	unlock_mount_hash();
	namespace_unlock();
}