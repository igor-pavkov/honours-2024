static void namespace_unlock(void)
{
	struct mount *mnt;
	struct hlist_head head = unmounted;

	if (likely(hlist_empty(&head))) {
		up_write(&namespace_sem);
		return;
	}

	head.first->pprev = &head.first;
	INIT_HLIST_HEAD(&unmounted);

	/* undo decrements we'd done in umount_tree() */
	hlist_for_each_entry(mnt, &head, mnt_hash)
		if (mnt->mnt_ex_mountpoint.mnt)
			mntget(mnt->mnt_ex_mountpoint.mnt);

	up_write(&namespace_sem);

	synchronize_rcu();

	while (!hlist_empty(&head)) {
		mnt = hlist_entry(head.first, struct mount, mnt_hash);
		hlist_del_init(&mnt->mnt_hash);
		if (mnt->mnt_ex_mountpoint.mnt)
			path_put(&mnt->mnt_ex_mountpoint);
		mntput(&mnt->mnt);
	}
}