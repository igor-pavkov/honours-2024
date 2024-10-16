static int shadow_copy2_fstat(vfs_handle_struct *handle, files_struct *fsp,
			      SMB_STRUCT_STAT *sbuf)
{
	time_t timestamp;
	int ret;

	ret = SMB_VFS_NEXT_FSTAT(handle, fsp, sbuf);
	if (ret == -1) {
		return ret;
	}
	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle,
					 fsp->fsp_name->base_name,
					 &timestamp, NULL)) {
		return 0;
	}
	if (timestamp != 0) {
		convert_sbuf(handle, fsp->fsp_name->base_name, sbuf);
	}
	return 0;
}