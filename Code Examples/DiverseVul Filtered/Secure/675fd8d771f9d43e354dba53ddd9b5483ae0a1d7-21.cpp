static int shadow_copy2_rename(vfs_handle_struct *handle,
			       const struct smb_filename *smb_fname_src,
			       const struct smb_filename *smb_fname_dst)
{
	time_t timestamp_src, timestamp_dst;

	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle,
					 smb_fname_src->base_name,
					 &timestamp_src, NULL)) {
		return -1;
	}
	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle,
					 smb_fname_dst->base_name,
					 &timestamp_dst, NULL)) {
		return -1;
	}
	if (timestamp_src != 0) {
		errno = EXDEV;
		return -1;
	}
	if (timestamp_dst != 0) {
		errno = EROFS;
		return -1;
	}
	return SMB_VFS_NEXT_RENAME(handle, smb_fname_src, smb_fname_dst);
}