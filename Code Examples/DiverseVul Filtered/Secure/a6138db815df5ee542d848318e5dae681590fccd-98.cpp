int may_umount_tree(struct vfsmount *m)
{
	struct mount *mnt = real_mount(m);
	int actual_refs = 0;
	int minimum_refs = 0;
	struct mount *p;
	BUG_ON(!m);

	/* write lock needed for mnt_get_count */
	lock_mount_hash();
	for (p = mnt; p; p = next_mnt(p, mnt)) {
		actual_refs += mnt_get_count(p);
		minimum_refs += 2;
	}
	unlock_mount_hash();

	if (actual_refs > minimum_refs)
		return 0;

	return 1;
}