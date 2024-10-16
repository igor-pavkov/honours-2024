static void attach_mnt(struct mount *mnt,
			struct mount *parent,
			struct mountpoint *mp)
{
	mnt_set_mountpoint(parent, mp, mnt);
	hlist_add_head_rcu(&mnt->mnt_hash, m_hash(&parent->mnt, mp->m_dentry));
	list_add_tail(&mnt->mnt_child, &parent->mnt_mounts);
}