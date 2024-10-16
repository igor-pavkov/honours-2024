static struct mountpoint *new_mountpoint(struct dentry *dentry)
{
	struct hlist_head *chain = mp_hash(dentry);
	struct mountpoint *mp;
	int ret;

	hlist_for_each_entry(mp, chain, m_hash) {
		if (mp->m_dentry == dentry) {
			/* might be worth a WARN_ON() */
			if (d_unlinked(dentry))
				return ERR_PTR(-ENOENT);
			mp->m_count++;
			return mp;
		}
	}

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
	return mp;
}