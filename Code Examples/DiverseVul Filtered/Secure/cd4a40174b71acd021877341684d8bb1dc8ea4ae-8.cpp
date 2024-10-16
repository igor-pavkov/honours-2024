static struct mountpoint *lookup_mountpoint(struct dentry *dentry)
{
	struct hlist_head *chain = mp_hash(dentry);
	struct mountpoint *mp;

	hlist_for_each_entry(mp, chain, m_hash) {
		if (mp->m_dentry == dentry) {
			/* might be worth a WARN_ON() */
			if (d_unlinked(dentry))
				return ERR_PTR(-ENOENT);
			mp->m_count++;
			return mp;
		}
	}
	return NULL;
}