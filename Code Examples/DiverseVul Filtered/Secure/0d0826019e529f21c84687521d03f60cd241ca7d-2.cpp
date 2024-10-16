static void attach_shadowed(struct mount *mnt,
			struct mount *parent,
			struct mount *shadows)
{
	if (shadows) {
		hlist_add_behind_rcu(&mnt->mnt_hash, &shadows->mnt_hash);
		list_add(&mnt->mnt_child, &shadows->mnt_child);
	} else {
		hlist_add_head_rcu(&mnt->mnt_hash,
				m_hash(&parent->mnt, mnt->mnt_mountpoint));
		list_add_tail(&mnt->mnt_child, &parent->mnt_mounts);
	}
}