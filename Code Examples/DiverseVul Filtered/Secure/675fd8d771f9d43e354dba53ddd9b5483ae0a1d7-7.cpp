static int shadow_copy2_symlink(vfs_handle_struct *handle,
				const char *oldname, const char *newname)
{
	time_t timestamp_old, timestamp_new;

	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle, oldname,
					 &timestamp_old, NULL)) {
		return -1;
	}
	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle, newname,
					 &timestamp_new, NULL)) {
		return -1;
	}
	if ((timestamp_old != 0) || (timestamp_new != 0)) {
		errno = EROFS;
		return -1;
	}
	return SMB_VFS_NEXT_SYMLINK(handle, oldname, newname);
}