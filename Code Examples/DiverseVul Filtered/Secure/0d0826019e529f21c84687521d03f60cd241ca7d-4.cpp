void umount_tree(struct mount *mnt, int how)
{
	HLIST_HEAD(tmp_list);
	struct mount *p;
	struct mount *last = NULL;

	for (p = mnt; p; p = next_mnt(p, mnt)) {
		hlist_del_init_rcu(&p->mnt_hash);
		hlist_add_head(&p->mnt_hash, &tmp_list);
	}

	hlist_for_each_entry(p, &tmp_list, mnt_hash)
		list_del_init(&p->mnt_child);

	if (how)
		propagate_umount(&tmp_list);

	hlist_for_each_entry(p, &tmp_list, mnt_hash) {
		list_del_init(&p->mnt_expire);
		list_del_init(&p->mnt_list);
		__touch_mnt_namespace(p->mnt_ns);
		p->mnt_ns = NULL;
		if (how < 2)
			p->mnt.mnt_flags |= MNT_SYNC_UMOUNT;
		if (mnt_has_parent(p)) {
			put_mountpoint(p->mnt_mp);
			mnt_add_count(p->mnt_parent, -1);
			/* move the reference to mountpoint into ->mnt_ex_mountpoint */
			p->mnt_ex_mountpoint.dentry = p->mnt_mountpoint;
			p->mnt_ex_mountpoint.mnt = &p->mnt_parent->mnt;
			p->mnt_mountpoint = p->mnt.mnt_root;
			p->mnt_parent = p;
			p->mnt_mp = NULL;
		}
		change_mnt_propagation(p, MS_PRIVATE);
		last = p;
	}
	if (last) {
		last->mnt_hash.next = unmounted.first;
		unmounted.first = tmp_list.first;
		unmounted.first->pprev = &unmounted.first;
	}
}