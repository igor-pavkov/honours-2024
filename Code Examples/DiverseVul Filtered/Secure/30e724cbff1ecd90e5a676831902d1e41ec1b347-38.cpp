static bool vfswrap_is_offline(struct vfs_handle_struct *handle,
			       const struct smb_filename *fname,
			       SMB_STRUCT_STAT *sbuf)
{
	NTSTATUS status;
	char *path;
	bool offline = false;

        if (ISDOT(fname->base_name) || ISDOTDOT(fname->base_name)) {
		return false;
	}

	if (!lp_dmapi_support(SNUM(handle->conn)) || !dmapi_have_session()) {
#if defined(ENOTSUP)
		errno = ENOTSUP;
#endif
		return false;
	}

        status = get_full_smb_filename(talloc_tos(), fname, &path);
        if (!NT_STATUS_IS_OK(status)) {
                errno = map_errno_from_nt_status(status);
                return false;
        }

	offline = (dmapi_file_flags(path) & FILE_ATTRIBUTE_OFFLINE) != 0;

	TALLOC_FREE(path);

	return offline;
}