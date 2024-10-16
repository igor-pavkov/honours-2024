static int shadow_copy2_unlink(vfs_handle_struct *handle,
			       const struct smb_filename *smb_fname)
{
	time_t timestamp;
	char *stripped;
	int ret, saved_errno;
	struct smb_filename *conv;

	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle,
					 smb_fname->base_name,
					 &timestamp, &stripped)) {
		return -1;
	}
	if (timestamp == 0) {
		return SMB_VFS_NEXT_UNLINK(handle, smb_fname);
	}
	conv = cp_smb_filename(talloc_tos(), smb_fname);
	if (conv == NULL) {
		errno = ENOMEM;
		return -1;
	}
	conv->base_name = shadow_copy2_convert(
		conv, handle, stripped, timestamp);
	TALLOC_FREE(stripped);
	if (conv->base_name == NULL) {
		return -1;
	}
	ret = SMB_VFS_NEXT_UNLINK(handle, conv);
	saved_errno = errno;
	TALLOC_FREE(conv);
	errno = saved_errno;
	return ret;
}