struct mount *__lookup_mnt_last(struct vfsmount *mnt, struct dentry *dentry)
{
	struct mount *p, *res;
	res = p = __lookup_mnt(mnt, dentry);
	if (!p)
		goto out;
	hlist_for_each_entry_continue(p, mnt_hash) {
		if (&p->mnt_parent->mnt != mnt || p->mnt_mountpoint != dentry)
			break;
		res = p;
	}
out:
	return res;
}