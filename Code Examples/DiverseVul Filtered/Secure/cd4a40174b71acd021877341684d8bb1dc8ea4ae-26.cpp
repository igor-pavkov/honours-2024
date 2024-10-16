static struct mountpoint *new_mountpoint(struct dentry *dentry)
{
	struct hlist_head *chain = mp_hash(dentry);
	struct mountpoint *mp;
	int ret;

	mp = kmalloc(sizeof(struct mountpoint), GFP_KERNEL);
	if (!mp)
		return ERR_PTR(-ENOMEM);

	ret = d_set_mounted(dentry);
	if (ret) {
		kfree(mp);
		return ERR_PTR(ret);
	}

	mp->m_dentry = dentry;
	mp->m_count = 1;
	hlist_add_head(&mp->m_hash, chain);
	INIT_HLIST_HEAD(&mp->m_list);
	return mp;
}