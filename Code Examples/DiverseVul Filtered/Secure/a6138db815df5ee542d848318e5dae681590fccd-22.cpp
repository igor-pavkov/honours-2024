void mnt_set_expiry(struct vfsmount *mnt, struct list_head *expiry_list)
{
	namespace_lock();

	list_add_tail(&real_mount(mnt)->mnt_expire, expiry_list);

	namespace_unlock();
}