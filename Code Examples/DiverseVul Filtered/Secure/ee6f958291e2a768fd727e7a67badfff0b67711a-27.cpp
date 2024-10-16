static void expire_mount(struct vfsmount *mnt, struct list_head *mounts,
				struct list_head *umounts)
{
	spin_lock(&vfsmount_lock);

	/*
	 * Check if mount is still attached, if not, let whoever holds it deal
	 * with the sucker
	 */
	if (mnt->mnt_parent == mnt) {
		spin_unlock(&vfsmount_lock);
		return;
	}

	/*
	 * Check that it is still dead: the count should now be 2 - as
	 * contributed by the vfsmount parent and the mntget above
	 */
	if (!propagate_mount_busy(mnt, 2)) {
		/* delete from the namespace */
		touch_mnt_namespace(mnt->mnt_ns);
		list_del_init(&mnt->mnt_list);
		mnt->mnt_ns = NULL;
		umount_tree(mnt, 1, umounts);
		spin_unlock(&vfsmount_lock);
	} else {
		/*
		 * Someone brought it back to life whilst we didn't have any
		 * locks held so return it to the expiration list
		 */
		list_add_tail(&mnt->mnt_expire, mounts);
		spin_unlock(&vfsmount_lock);
	}
}