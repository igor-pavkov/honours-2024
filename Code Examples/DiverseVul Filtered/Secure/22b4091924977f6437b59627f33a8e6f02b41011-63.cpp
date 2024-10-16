static void call_trans2qfilepathinfo(connection_struct *conn,
				     struct smb_request *req,
				     unsigned int tran_call,
				     char **pparams, int total_params,
				     char **ppdata, int total_data,
				     unsigned int max_data_bytes)
{
	char *params = *pparams;
	char *pdata = *ppdata;
	uint16_t info_level;
	unsigned int data_size = 0;
	unsigned int param_size = 2;
	struct smb_filename *smb_fname = NULL;
	bool delete_pending = False;
	struct timespec write_time_ts;
	files_struct *fsp = NULL;
	struct file_id fileid;
	struct ea_list *ea_list = NULL;
	int lock_data_count = 0;
	char *lock_data = NULL;
	size_t fixed_portion;
	NTSTATUS status = NT_STATUS_OK;

	if (!params) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	ZERO_STRUCT(write_time_ts);

	if (tran_call == TRANSACT2_QFILEINFO) {
		if (total_params < 4) {
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			return;
		}

		if (IS_IPC(conn)) {
			call_trans2qpipeinfo(conn, req,	tran_call,
					     pparams, total_params,
					     ppdata, total_data,
					     max_data_bytes);
			return;
		}

		fsp = file_fsp(req, SVAL(params,0));
		info_level = SVAL(params,2);

		DEBUG(3,("call_trans2qfilepathinfo: TRANSACT2_QFILEINFO: level = %d\n", info_level));

		if (INFO_LEVEL_IS_UNIX(info_level) && !lp_unix_extensions()) {
			reply_nterror(req, NT_STATUS_INVALID_LEVEL);
			return;
		}

		/* Initial check for valid fsp ptr. */
		if (!check_fsp_open(conn, req, fsp)) {
			return;
		}

		smb_fname = fsp->fsp_name;

		if(fsp->fake_file_handle) {
			/*
			 * This is actually for the QUOTA_FAKE_FILE --metze
			 */

			/* We know this name is ok, it's already passed the checks. */

		} else if(fsp_get_pathref_fd(fsp) == -1) {
			/*
			 * This is actually a QFILEINFO on a directory
			 * handle (returned from an NT SMB). NT5.0 seems
			 * to do this call. JRA.
			 */

			if (INFO_LEVEL_IS_UNIX(info_level)) {
				/* Always do lstat for UNIX calls. */
				if (SMB_VFS_LSTAT(conn, smb_fname)) {
					DEBUG(3,("call_trans2qfilepathinfo: "
						 "SMB_VFS_LSTAT of %s failed "
						 "(%s)\n",
						 smb_fname_str_dbg(smb_fname),
						 strerror(errno)));
					reply_nterror(req,
						map_nt_error_from_unix(errno));
					return;
				}
			} else if (SMB_VFS_STAT(conn, smb_fname)) {
				DEBUG(3,("call_trans2qfilepathinfo: "
					 "SMB_VFS_STAT of %s failed (%s)\n",
					 smb_fname_str_dbg(smb_fname),
					 strerror(errno)));
				reply_nterror(req,
					map_nt_error_from_unix(errno));
				return;
			}

			if (lp_smbd_getinfo_ask_sharemode(SNUM(conn))) {
				fileid = vfs_file_id_from_sbuf(
					conn, &smb_fname->st);
				get_file_infos(fileid, fsp->name_hash,
					       &delete_pending,
					       &write_time_ts);
			}
		} else {
			/*
			 * Original code - this is an open file.
			 */
			if (SMB_VFS_FSTAT(fsp, &smb_fname->st) != 0) {
				DEBUG(3, ("fstat of %s failed (%s)\n",
					  fsp_fnum_dbg(fsp), strerror(errno)));
				reply_nterror(req,
					map_nt_error_from_unix(errno));
				return;
			}
			if (lp_smbd_getinfo_ask_sharemode(SNUM(conn))) {
				fileid = vfs_file_id_from_sbuf(
					conn, &smb_fname->st);
				get_file_infos(fileid, fsp->name_hash,
					       &delete_pending,
					       &write_time_ts);
			}
		}

	} else {
		uint32_t name_hash;
		char *fname = NULL;
		uint32_t ucf_flags = ucf_flags_from_smb_request(req);

		/* qpathinfo */
		if (total_params < 7) {
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			return;
		}

		info_level = SVAL(params,0);

		DEBUG(3,("call_trans2qfilepathinfo: TRANSACT2_QPATHINFO: level = %d\n", info_level));

		if (INFO_LEVEL_IS_UNIX(info_level)) {
			if (!lp_unix_extensions()) {
				reply_nterror(req, NT_STATUS_INVALID_LEVEL);
				return;
			}
		}

		if (req->posix_pathnames) {
			srvstr_get_path_posix(req,
				params,
				req->flags2,
				&fname,
				&params[6],
				total_params - 6,
				STR_TERMINATE,
				&status);
		} else {
			srvstr_get_path(req,
				params,
				req->flags2,
				&fname,
				&params[6],
				total_params - 6,
				STR_TERMINATE,
				&status);
		}
		if (!NT_STATUS_IS_OK(status)) {
			reply_nterror(req, status);
			return;
		}

		status = filename_convert(req,
					conn,
					fname,
					ucf_flags,
					0,
					&smb_fname);
		if (!NT_STATUS_IS_OK(status)) {
			if (NT_STATUS_EQUAL(status,NT_STATUS_PATH_NOT_COVERED)) {
				reply_botherror(req,
						NT_STATUS_PATH_NOT_COVERED,
						ERRSRV, ERRbadpath);
				return;
			}
			reply_nterror(req, status);
			return;
		}

		/*
		 * smb_fname->fsp may be NULL if smb_fname points at a symlink
		 * and we're in POSIX context, so be careful when using fsp
		 * below, it can still be NULL.
		 */
		fsp = smb_fname->fsp;

		/* If this is a stream, check if there is a delete_pending. */
		if ((conn->fs_capabilities & FILE_NAMED_STREAMS)
		    && is_ntfs_stream_smb_fname(smb_fname)) {
			struct smb_filename *smb_fname_base;

			/* Create an smb_filename with stream_name == NULL. */
			smb_fname_base = synthetic_smb_fname(
						talloc_tos(),
						smb_fname->base_name,
						NULL,
						NULL,
						smb_fname->twrp,
						smb_fname->flags);
			if (smb_fname_base == NULL) {
				reply_nterror(req, NT_STATUS_NO_MEMORY);
				return;
			}

			if (INFO_LEVEL_IS_UNIX(info_level) || req->posix_pathnames) {
				/* Always do lstat for UNIX calls. */
				if (SMB_VFS_LSTAT(conn, smb_fname_base) != 0) {
					DEBUG(3,("call_trans2qfilepathinfo: "
						 "SMB_VFS_LSTAT of %s failed "
						 "(%s)\n",
						 smb_fname_str_dbg(smb_fname_base),
						 strerror(errno)));
					TALLOC_FREE(smb_fname_base);
					reply_nterror(req,
						map_nt_error_from_unix(errno));
					return;
				}
			} else {
				if (SMB_VFS_STAT(conn, smb_fname_base) != 0) {
					DEBUG(3,("call_trans2qfilepathinfo: "
						 "fileinfo of %s failed "
						 "(%s)\n",
						 smb_fname_str_dbg(smb_fname_base),
						 strerror(errno)));
					TALLOC_FREE(smb_fname_base);
					reply_nterror(req,
						map_nt_error_from_unix(errno));
					return;
				}
			}

			status = file_name_hash(conn,
					smb_fname_str_dbg(smb_fname_base),
					&name_hash);
			if (!NT_STATUS_IS_OK(status)) {
				TALLOC_FREE(smb_fname_base);
				reply_nterror(req, status);
				return;
			}

			fileid = vfs_file_id_from_sbuf(conn,
						       &smb_fname_base->st);
			TALLOC_FREE(smb_fname_base);
			get_file_infos(fileid, name_hash, &delete_pending, NULL);
			if (delete_pending) {
				reply_nterror(req, NT_STATUS_DELETE_PENDING);
				return;
			}
		}

		if (INFO_LEVEL_IS_UNIX(info_level) || req->posix_pathnames) {
			/* Always do lstat for UNIX calls. */
			if (SMB_VFS_LSTAT(conn, smb_fname)) {
				DEBUG(3,("call_trans2qfilepathinfo: "
					 "SMB_VFS_LSTAT of %s failed (%s)\n",
					 smb_fname_str_dbg(smb_fname),
					 strerror(errno)));
				reply_nterror(req,
					map_nt_error_from_unix(errno));
				return;
			}

		} else {
			if (SMB_VFS_STAT(conn, smb_fname) != 0) {
				DEBUG(3,("call_trans2qfilepathinfo: "
					 "SMB_VFS_STAT of %s failed (%s)\n",
					 smb_fname_str_dbg(smb_fname),
					 strerror(errno)));
				reply_nterror(req,
					map_nt_error_from_unix(errno));
				return;
			}
		}

		status = file_name_hash(conn,
				smb_fname_str_dbg(smb_fname),
				&name_hash);
		if (!NT_STATUS_IS_OK(status)) {
			reply_nterror(req, status);
			return;
		}

		if (lp_smbd_getinfo_ask_sharemode(SNUM(conn))) {
			fileid = vfs_file_id_from_sbuf(conn, &smb_fname->st);
			get_file_infos(fileid, name_hash, &delete_pending,
				       &write_time_ts);
		}

		if (delete_pending) {
			reply_nterror(req, NT_STATUS_DELETE_PENDING);
			return;
		}
	}

	DEBUG(3,("call_trans2qfilepathinfo %s (%s) level=%d call=%d "
		 "total_data=%d\n", smb_fname_str_dbg(smb_fname),
		 fsp_fnum_dbg(fsp),
		 info_level,tran_call,total_data));

	/* Pull out any data sent here before we realloc. */
	switch (info_level) {
		case SMB_INFO_QUERY_EAS_FROM_LIST:
		{
			/* Pull any EA list from the data portion. */
			uint32_t ea_size;

			if (total_data < 4) {
				reply_nterror(
					req, NT_STATUS_INVALID_PARAMETER);
				return;
			}
			ea_size = IVAL(pdata,0);

			if (total_data > 0 && ea_size != total_data) {
				DEBUG(4,("call_trans2qfilepathinfo: Rejecting EA request with incorrect \
total_data=%u (should be %u)\n", (unsigned int)total_data, (unsigned int)IVAL(pdata,0) ));
				reply_nterror(
					req, NT_STATUS_INVALID_PARAMETER);
				return;
			}

			if (!lp_ea_support(SNUM(conn))) {
				reply_nterror(req, NT_STATUS_EAS_NOT_SUPPORTED);
				return;
			}

			/* Pull out the list of names. */
			ea_list = read_ea_name_list(req, pdata + 4, ea_size - 4);
			if (!ea_list) {
				reply_nterror(
					req, NT_STATUS_INVALID_PARAMETER);
				return;
			}
			break;
		}

		case SMB_QUERY_POSIX_LOCK:
		{
			if (fsp == NULL ||
			    fsp->fsp_flags.is_pathref ||
			    fsp_get_io_fd(fsp) == -1)
			{
				reply_nterror(req, NT_STATUS_INVALID_HANDLE);
				return;
			}

			if (total_data != POSIX_LOCK_DATA_SIZE) {
				reply_nterror(
					req, NT_STATUS_INVALID_PARAMETER);
				return;
			}

			/* Copy the lock range data. */
			lock_data = (char *)talloc_memdup(
				req, pdata, total_data);
			if (!lock_data) {
				reply_nterror(req, NT_STATUS_NO_MEMORY);
				return;
			}
			lock_data_count = total_data;
			break;
		}
		default:
			break;
	}

	*pparams = (char *)SMB_REALLOC(*pparams,2);
	if (*pparams == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		return;
	}
	params = *pparams;
	SSVAL(params,0,0);

	/*
	 * draft-leach-cifs-v1-spec-02.txt
	 * 4.2.14 TRANS2_QUERY_PATH_INFORMATION: Get File Attributes given Path
	 * says:
	 *
	 *  The requested information is placed in the Data portion of the
	 *  transaction response. For the information levels greater than 0x100,
	 *  the transaction response has 1 parameter word which should be
	 *  ignored by the client.
	 *
	 * However Windows only follows this rule for the IS_NAME_VALID call.
	 */
	switch (info_level) {
	case SMB_INFO_IS_NAME_VALID:
		param_size = 0;
		break;
	}

	if ((info_level & SMB2_INFO_SPECIAL) == SMB2_INFO_SPECIAL) {
		/*
		 * We use levels that start with 0xFF00
		 * internally to represent SMB2 specific levels
		 */
		reply_nterror(req, NT_STATUS_INVALID_LEVEL);
		return;
	}

	status = smbd_do_qfilepathinfo(conn, req, req, info_level,
				       fsp, smb_fname,
				       delete_pending, write_time_ts,
				       ea_list,
				       lock_data_count, lock_data,
				       req->flags2, max_data_bytes,
				       &fixed_portion,
				       ppdata, &data_size);
	if (!NT_STATUS_IS_OK(status)) {
		if (open_was_deferred(req->xconn, req->mid)) {
			/* We have re-scheduled this call. */
			return;
		}
		if (NT_STATUS_EQUAL(status, NT_STATUS_SHARING_VIOLATION)) {
			bool ok = defer_smb1_sharing_violation(req);
			if (ok) {
				return;
			}
		}
		reply_nterror(req, status);
		return;
	}
	if (fixed_portion > max_data_bytes) {
		reply_nterror(req, NT_STATUS_INFO_LENGTH_MISMATCH);
		return;
	}

	send_trans2_replies(conn, req, NT_STATUS_OK, params, param_size, *ppdata, data_size,
			    max_data_bytes);

	return;
}