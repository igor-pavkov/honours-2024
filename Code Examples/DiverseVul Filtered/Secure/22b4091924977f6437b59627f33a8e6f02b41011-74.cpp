static NTSTATUS smb_set_file_unix_info2(connection_struct *conn,
					struct smb_request *req,
					const char *pdata,
					int total_data,
					files_struct *fsp,
					struct smb_filename *smb_fname)
{
	NTSTATUS status;
	uint32_t smb_fflags;
	uint32_t smb_fmask;

	if (total_data < 116) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	/* Start by setting all the fields that are common between UNIX_BASIC
	 * and UNIX_INFO2.
	 */
	status = smb_set_file_unix_basic(conn, req, pdata, total_data,
					 fsp, smb_fname);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	smb_fflags = IVAL(pdata, 108);
	smb_fmask = IVAL(pdata, 112);

	/* NB: We should only attempt to alter the file flags if the client
	 * sends a non-zero mask.
	 */
	if (smb_fmask != 0) {
		int stat_fflags = 0;

		if (!map_info2_flags_to_sbuf(&smb_fname->st, smb_fflags,
					     smb_fmask, &stat_fflags)) {
			/* Client asked to alter a flag we don't understand. */
			return NT_STATUS_INVALID_PARAMETER;
		}

		if (fsp == NULL || S_ISLNK(smb_fname->st.st_ex_mode)) {
			DBG_WARNING("Can't change flags on symlink %s\n",
				smb_fname_str_dbg(smb_fname));
			return NT_STATUS_OBJECT_NAME_NOT_FOUND;
		}
		if (SMB_VFS_FCHFLAGS(fsp, stat_fflags) != 0) {
			return map_nt_error_from_unix(errno);
		}
	}

	/* XXX: need to add support for changing the create_time here. You
	 * can do this for paths on Darwin with setattrlist(2). The right way
	 * to hook this up is probably by extending the VFS utimes interface.
	 */

	return NT_STATUS_OK;
}