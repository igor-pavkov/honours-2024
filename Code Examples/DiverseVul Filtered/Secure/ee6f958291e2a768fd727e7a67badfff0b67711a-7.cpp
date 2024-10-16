void mark_mounts_for_expiry(struct list_head *mounts)
{
	struct vfsmount *mnt, *next;
	LIST_HEAD(graveyard);

	if (list_empty(mounts))
		return;

	spin_lock(&vfsmount_lock);

	/* extract from the expiration list every vfsmount that matches the
	 * following criteria:
	 * - only referenced by its parent vfsmount
	 * - still marked for expiry (marked on the last call here; marks are
	 *   cleared by mntput())
	 */
	list_for_each_entry_safe(mnt, next, mounts, mnt_expire) {
		if (!xchg(&mnt->mnt_expiry_mark, 1) ||
		    atomic_read(&mnt->mnt_count) != 1)
			continue;

		mntget(mnt);
		list_move(&mnt->mnt_expire, &graveyard);
	}

	expire_mount_list(&graveyard, mounts);

	spin_unlock(&vfsmount_lock);
}