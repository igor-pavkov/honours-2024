bool legitimize_mnt(struct vfsmount *bastard, unsigned seq)
{
	struct mount *mnt;
	if (read_seqretry(&mount_lock, seq))
		return false;
	if (bastard == NULL)
		return true;
	mnt = real_mount(bastard);
	mnt_add_count(mnt, 1);
	if (likely(!read_seqretry(&mount_lock, seq)))
		return true;
	if (bastard->mnt_flags & MNT_SYNC_UMOUNT) {
		mnt_add_count(mnt, -1);
		return false;
	}
	rcu_read_unlock();
	mntput(bastard);
	rcu_read_lock();
	return false;
}