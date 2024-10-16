static NTSTATUS smb_set_file_allocation_info(connection_struct *conn,
					     struct smb_request *req,
					const char *pdata,
					int total_data,
					files_struct *fsp,
					struct smb_filename *smb_fname)
{
	uint64_t allocation_size = 0;
	NTSTATUS status = NT_STATUS_OK;
	files_struct *new_fsp = NULL;

	if (!VALID_STAT(smb_fname->st)) {
		return NT_STATUS_OBJECT_NAME_NOT_FOUND;
	}

	if (total_data < 8) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	allocation_size = (uint64_t)IVAL(pdata,0);
	allocation_size |= (((uint64_t)IVAL(pdata,4)) << 32);
	DEBUG(10,("smb_set_file_allocation_info: Set file allocation info for "
		  "file %s to %.0f\n", smb_fname_str_dbg(smb_fname),
		  (double)allocation_size));

	if (allocation_size) {
		allocation_size = smb_roundup(conn, allocation_size);
	}

	DEBUG(10,("smb_set_file_allocation_info: file %s : setting new "
		  "allocation size to %.0f\n", smb_fname_str_dbg(smb_fname),
		  (double)allocation_size));

	if (fsp &&
	    !fsp->fsp_flags.is_pathref &&
	    fsp_get_io_fd(fsp) != -1)
	{
		/* Open file handle. */
		if (!(fsp->access_mask & FILE_WRITE_DATA)) {
			return NT_STATUS_ACCESS_DENIED;
		}

		/* Only change if needed. */
		if (allocation_size != get_file_size_stat(&smb_fname->st)) {
			if (vfs_allocate_file_space(fsp, allocation_size) == -1) {
				return map_nt_error_from_unix(errno);
			}
		}
		/* But always update the time. */
		/*
		 * This is equivalent to a write. Ensure it's seen immediately
		 * if there are no pending writes.
		 */
		trigger_write_time_update_immediate(fsp);
		return NT_STATUS_OK;
	}

	/* Pathname or stat or directory file. */
	status = SMB_VFS_CREATE_FILE(
		conn,					/* conn */
		req,					/* req */
		smb_fname,				/* fname */
		FILE_WRITE_DATA,			/* access_mask */
		(FILE_SHARE_READ | FILE_SHARE_WRITE |	/* share_access */
		    FILE_SHARE_DELETE),
		FILE_OPEN,				/* create_disposition*/
		0,					/* create_options */
		FILE_ATTRIBUTE_NORMAL,			/* file_attributes */
		0,					/* oplock_request */
		NULL,					/* lease */
		0,					/* allocation_size */
		0,					/* private_flags */
		NULL,					/* sd */
		NULL,					/* ea_list */
		&new_fsp,				/* result */
		NULL,					/* pinfo */
		NULL, NULL);				/* create context */

	if (!NT_STATUS_IS_OK(status)) {
		/* NB. We check for open_was_deferred in the caller. */
		return status;
	}

	/* Only change if needed. */
	if (allocation_size != get_file_size_stat(&smb_fname->st)) {
		if (vfs_allocate_file_space(new_fsp, allocation_size) == -1) {
			status = map_nt_error_from_unix(errno);
			close_file(req, new_fsp, NORMAL_CLOSE);
			return status;
		}
	}

	/* Changing the allocation size should set the last mod time. */
	/*
	 * This is equivalent to a write. Ensure it's seen immediately
	 * if there are no pending writes.
	 */
	trigger_write_time_update_immediate(new_fsp);
	close_file(req, new_fsp, NORMAL_CLOSE);
	return NT_STATUS_OK;
}