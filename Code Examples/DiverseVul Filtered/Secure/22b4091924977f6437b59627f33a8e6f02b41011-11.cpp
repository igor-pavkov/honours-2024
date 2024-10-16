NTSTATUS set_ea(connection_struct *conn, files_struct *fsp,
		struct ea_list *ea_list)
{
	NTSTATUS status;
	bool posix_pathnames = false;

	if (!lp_ea_support(SNUM(conn))) {
		return NT_STATUS_EAS_NOT_SUPPORTED;
	}

	if (fsp == NULL) {
		return NT_STATUS_INVALID_HANDLE;
	}

	posix_pathnames = (fsp->fsp_name->flags & SMB_FILENAME_POSIX_PATH);

	status = refuse_symlink_fsp(fsp);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	status = check_access_fsp(fsp, FILE_WRITE_EA);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/* Setting EAs on streams isn't supported. */
	if (is_ntfs_stream_smb_fname(fsp->fsp_name)) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	/*
	 * Filter out invalid Windows EA names - before
	 * we set *any* of them.
	 */

	if (!posix_pathnames && ea_list_has_invalid_name(ea_list)) {
		return STATUS_INVALID_EA_NAME;
	}

	for (;ea_list; ea_list = ea_list->next) {
		int ret;
		fstring unix_ea_name;

		fstrcpy(unix_ea_name, "user."); /* All EA's must start with user. */
		fstrcat(unix_ea_name, ea_list->ea.name);

		canonicalize_ea_name(fsp, unix_ea_name);

		DEBUG(10,("set_ea: ea_name %s ealen = %u\n", unix_ea_name, (unsigned int)ea_list->ea.value.length));

		if (samba_private_attr_name(unix_ea_name)) {
			DEBUG(10,("set_ea: ea name %s is a private Samba name.\n", unix_ea_name));
			return NT_STATUS_ACCESS_DENIED;
		}

		if (ea_list->ea.value.length == 0) {
			/* Remove the attribute. */
			DBG_DEBUG("deleting ea name %s on "
				  "file %s by file descriptor.\n",
				  unix_ea_name, fsp_str_dbg(fsp));
			ret = SMB_VFS_FREMOVEXATTR(fsp, unix_ea_name);
#ifdef ENOATTR
			/* Removing a non existent attribute always succeeds. */
			if (ret == -1 && errno == ENOATTR) {
				DEBUG(10,("set_ea: deleting ea name %s didn't exist - succeeding by default.\n",
						unix_ea_name));
				ret = 0;
			}
#endif
		} else {
			DEBUG(10,("set_ea: setting ea name %s on file "
				  "%s by file descriptor.\n",
				  unix_ea_name, fsp_str_dbg(fsp)));
			ret = SMB_VFS_FSETXATTR(fsp, unix_ea_name,
						ea_list->ea.value.data, ea_list->ea.value.length, 0);
		}

		if (ret == -1) {
#ifdef ENOTSUP
			if (errno == ENOTSUP) {
				return NT_STATUS_EAS_NOT_SUPPORTED;
			}
#endif
			return map_nt_error_from_unix(errno);
		}

	}
	return NT_STATUS_OK;
}