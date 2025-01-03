static int shadow_copy2_open(vfs_handle_struct *handle,
			     struct smb_filename *smb_fname, files_struct *fsp,
			     int flags, mode_t mode)
{
	time_t timestamp;
	char *stripped, *tmp;
	int ret, saved_errno;

	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle,
					 smb_fname->base_name,
					 &timestamp, &stripped)) {
		return -1;
	}
	if (timestamp == 0) {
		return SMB_VFS_NEXT_OPEN(handle, smb_fname, fsp, flags, mode);
	}

	tmp = smb_fname->base_name;
	smb_fname->base_name = shadow_copy2_convert(
		talloc_tos(), handle, stripped, timestamp);
	TALLOC_FREE(stripped);

	if (smb_fname->base_name == NULL) {
		smb_fname->base_name = tmp;
		return -1;
	}

	ret = SMB_VFS_NEXT_OPEN(handle, smb_fname, fsp, flags, mode);
	saved_errno = errno;

	TALLOC_FREE(smb_fname->base_name);
	smb_fname->base_name = tmp;

	errno = saved_errno;
	return ret;
}