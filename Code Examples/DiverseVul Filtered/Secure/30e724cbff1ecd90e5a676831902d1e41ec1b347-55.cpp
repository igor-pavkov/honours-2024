static uint64_t vfswrap_disk_free(vfs_handle_struct *handle, const char *path, bool small_query, uint64_t *bsize,
			       uint64_t *dfree, uint64_t *dsize)
{
	uint64_t result;

	result = sys_disk_free(handle->conn, path, small_query, bsize, dfree, dsize);
	return result;
}