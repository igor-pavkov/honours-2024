void shrink_submounts(struct vfsmount *mountpoint, struct list_head *mounts)
{
	LIST_HEAD(graveyard);
	int found;

	spin_lock(&vfsmount_lock);

	/* extract submounts of 'mountpoint' from the expiration list */
	while ((found = select_submounts(mountpoint, &graveyard)) != 0)
		expire_mount_list(&graveyard, mounts);

	spin_unlock(&vfsmount_lock);
}