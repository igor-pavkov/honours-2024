static bool is_native_overlayfs(const char *path)
{
	if (has_fs_type(path, OVERLAY_SUPER_MAGIC) ||
	    has_fs_type(path, OVERLAYFS_SUPER_MAGIC))
		return true;

	return false;
}