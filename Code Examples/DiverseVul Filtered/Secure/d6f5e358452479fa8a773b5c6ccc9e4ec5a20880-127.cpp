void close_cached_dir_lease(struct cached_fid *cfid)
{
	mutex_lock(&cfid->fid_mutex);
	close_cached_dir_lease_locked(cfid);
	mutex_unlock(&cfid->fid_mutex);
}