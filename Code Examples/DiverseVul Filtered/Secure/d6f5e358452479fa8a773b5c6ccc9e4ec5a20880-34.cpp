int open_cached_dir_by_dentry(struct cifs_tcon *tcon,
			      struct dentry *dentry,
			      struct cached_fid **cfid)
{
	mutex_lock(&tcon->crfid.fid_mutex);
	if (tcon->crfid.dentry == dentry) {
		cifs_dbg(FYI, "found a cached root file handle by dentry\n");
		*cfid = &tcon->crfid;
		kref_get(&tcon->crfid.refcount);
		mutex_unlock(&tcon->crfid.fid_mutex);
		return 0;
	}
	mutex_unlock(&tcon->crfid.fid_mutex);
	return -ENOENT;
}