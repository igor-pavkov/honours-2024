static void expire_mount_list(struct list_head *graveyard, struct list_head *mounts)
{
	struct mnt_namespace *ns;
	struct vfsmount *mnt;

	while (!list_empty(graveyard)) {
		LIST_HEAD(umounts);
		mnt = list_first_entry(graveyard, struct vfsmount, mnt_expire);
		list_del_init(&mnt->mnt_expire);

		/* don't do anything if the namespace is dead - all the
		 * vfsmounts from it are going away anyway */
		ns = mnt->mnt_ns;
		if (!ns || !ns->root)
			continue;
		get_mnt_ns(ns);

		spin_unlock(&vfsmount_lock);
		down_write(&namespace_sem);
		expire_mount(mnt, mounts, &umounts);
		up_write(&namespace_sem);
		release_mounts(&umounts);
		mntput(mnt);
		put_mnt_ns(ns);
		spin_lock(&vfsmount_lock);
	}
}