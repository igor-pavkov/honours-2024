void close_cached_dir_lease_locked(struct cached_fid *cfid)
{
	if (cfid->has_lease) {
		cfid->has_lease = false;
		kref_put(&cfid->refcount, smb2_close_cached_fid);
	}
}