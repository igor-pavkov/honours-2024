struct mount *copy_tree(struct mount *mnt, struct dentry *dentry,
					int flag)
{
	struct mount *res, *p, *q, *r, *parent;

	if (!(flag & CL_COPY_UNBINDABLE) && IS_MNT_UNBINDABLE(mnt))
		return ERR_PTR(-EINVAL);

	if (!(flag & CL_COPY_MNT_NS_FILE) && is_mnt_ns_file(dentry))
		return ERR_PTR(-EINVAL);

	res = q = clone_mnt(mnt, dentry, flag);
	if (IS_ERR(q))
		return q;

	q->mnt.mnt_flags &= ~MNT_LOCKED;
	q->mnt_mountpoint = mnt->mnt_mountpoint;

	p = mnt;
	list_for_each_entry(r, &mnt->mnt_mounts, mnt_child) {
		struct mount *s;
		if (!is_subdir(r->mnt_mountpoint, dentry))
			continue;

		for (s = r; s; s = next_mnt(s, r)) {
			struct mount *t = NULL;
			if (!(flag & CL_COPY_UNBINDABLE) &&
			    IS_MNT_UNBINDABLE(s)) {
				s = skip_mnt_tree(s);
				continue;
			}
			if (!(flag & CL_COPY_MNT_NS_FILE) &&
			    is_mnt_ns_file(s->mnt.mnt_root)) {
				s = skip_mnt_tree(s);
				continue;
			}
			while (p != s->mnt_parent) {
				p = p->mnt_parent;
				q = q->mnt_parent;
			}
			p = s;
			parent = q;
			q = clone_mnt(p, p->mnt.mnt_root, flag);
			if (IS_ERR(q))
				goto out;
			lock_mount_hash();
			list_add_tail(&q->mnt_list, &res->mnt_list);
			mnt_set_mountpoint(parent, p->mnt_mp, q);
			if (!list_empty(&parent->mnt_mounts)) {
				t = list_last_entry(&parent->mnt_mounts,
					struct mount, mnt_child);
				if (t->mnt_mp != p->mnt_mp)
					t = NULL;
			}
			attach_shadowed(q, parent, t);
			unlock_mount_hash();
		}
	}
	return res;
out:
	if (res) {
		lock_mount_hash();
		umount_tree(res, 0);
		unlock_mount_hash();
	}
	return q;
}