static void call_trans2setfilepathinfo(connection_struct *conn,
				       struct smb_request *req,
				       unsigned int tran_call,
				       char **pparams, int total_params,
				       char **ppdata, int total_data,
				       unsigned int max_data_bytes)
{
	char *params = *pparams;
	char *pdata = *ppdata;
	uint16_t info_level;
	struct smb_filename *smb_fname = NULL;
	files_struct *fsp = NULL;
	NTSTATUS status = NT_STATUS_OK;
	int data_return_size = 0;

	if (!params) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	if (tran_call == TRANSACT2_SETFILEINFO) {
		if (total_params < 4) {
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			return;
		}

		fsp = file_fsp(req, SVAL(params,0));
		/* Basic check for non-null fsp. */
	        if (!check_fsp_open(conn, req, fsp)) {
			return;
		}
		info_level = SVAL(params,2);

		smb_fname = fsp->fsp_name;

		if (fsp_get_pathref_fd(fsp) == -1) {
			/*
			 * This is actually a SETFILEINFO on a directory
			 * handle (returned from an NT SMB). NT5.0 seems
			 * to do this call. JRA.
			 */
			if (INFO_LEVEL_IS_UNIX(info_level)) {
				/* Always do lstat for UNIX calls. */
				if (SMB_VFS_LSTAT(conn, smb_fname)) {
					DEBUG(3,("call_trans2setfilepathinfo: "
						 "SMB_VFS_LSTAT of %s failed "
						 "(%s)\n",
						 smb_fname_str_dbg(smb_fname),
						 strerror(errno)));
					reply_nterror(req, map_nt_error_from_unix(errno));
					return;
				}
			} else {
				if (SMB_VFS_STAT(conn, smb_fname) != 0) {
					DEBUG(3,("call_trans2setfilepathinfo: "
						 "fileinfo of %s failed (%s)\n",
						 smb_fname_str_dbg(smb_fname),
						 strerror(errno)));
					reply_nterror(req, map_nt_error_from_unix(errno));
					return;
				}
			}
		} else if (fsp->print_file) {
			/*
			 * Doing a DELETE_ON_CLOSE should cancel a print job.
			 */
			if ((info_level == SMB_SET_FILE_DISPOSITION_INFO) && CVAL(pdata,0)) {
				uint32_t new_private_options =
					fh_get_private_options(fsp->fh);
				new_private_options |=
					NTCREATEX_FLAG_DELETE_ON_CLOSE;
				fh_set_private_options(fsp->fh,
						       new_private_options);

				DEBUG(3,("call_trans2setfilepathinfo: "
					 "Cancelling print job (%s)\n",
					 fsp_str_dbg(fsp)));

				SSVAL(params,0,0);
				send_trans2_replies(conn, req, NT_STATUS_OK, params, 2,
						    *ppdata, 0,
						    max_data_bytes);
				return;
			} else {
				reply_nterror(req,
					NT_STATUS_OBJECT_PATH_NOT_FOUND);
				return;
			}
		} else {
			/*
			 * Original code - this is an open file.
			 */
			if (SMB_VFS_FSTAT(fsp, &smb_fname->st) != 0) {
				DEBUG(3,("call_trans2setfilepathinfo: fstat "
					 "of %s failed (%s)\n", fsp_fnum_dbg(fsp),
					 strerror(errno)));
				reply_nterror(req, map_nt_error_from_unix(errno));
				return;
			}
		}
	} else {
		char *fname = NULL;
		uint32_t ucf_flags = ucf_flags_from_smb_request(req);

		/* set path info */
		if (total_params < 7) {
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			return;
		}

		info_level = SVAL(params,0);
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

		status = filename_convert(req, conn,
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

		if (INFO_LEVEL_IS_UNIX(info_level)) {
			/*
			 * For CIFS UNIX extensions the target name may not exist.
			 */

			/* Always do lstat for UNIX calls. */
			SMB_VFS_LSTAT(conn, smb_fname);

		} else if (!VALID_STAT(smb_fname->st) &&
			   SMB_VFS_STAT(conn, smb_fname)) {
			DEBUG(3,("call_trans2setfilepathinfo: SMB_VFS_STAT of "
				 "%s failed (%s)\n",
				 smb_fname_str_dbg(smb_fname),
				 strerror(errno)));
			reply_nterror(req, map_nt_error_from_unix(errno));
			return;
		}
	}

	DEBUG(3,("call_trans2setfilepathinfo(%d) %s (%s) info_level=%d "
		 "totdata=%d\n", tran_call, smb_fname_str_dbg(smb_fname),
		 fsp_fnum_dbg(fsp),
		 info_level,total_data));

	/* Realloc the parameter size */
	*pparams = (char *)SMB_REALLOC(*pparams,2);
	if (*pparams == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		return;
	}
	params = *pparams;

	SSVAL(params,0,0);

	status = smbd_do_setfilepathinfo(conn, req, req,
					 info_level,
					 fsp,
					 smb_fname,
					 ppdata, total_data,
					 &data_return_size);
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
		if (NT_STATUS_EQUAL(status, NT_STATUS_EVENT_PENDING)) {
			/* We have re-scheduled this call. */
			return;
		}
		if (NT_STATUS_EQUAL(status,NT_STATUS_PATH_NOT_COVERED)) {
			reply_botherror(req, NT_STATUS_PATH_NOT_COVERED,
					ERRSRV, ERRbadpath);
			return;
		}
		if (info_level == SMB_POSIX_PATH_OPEN) {
			reply_openerror(req, status);
			return;
		}

		/*
		 * Invalid EA name needs to return 2 param bytes,
		 * not a zero-length error packet.
		 */
		if (NT_STATUS_EQUAL(status, STATUS_INVALID_EA_NAME)) {
			send_trans2_replies(conn, req, status, params, 2, NULL, 0,
					max_data_bytes);
		} else {
			reply_nterror(req, status);
		}
		return;
	}

	send_trans2_replies(conn, req, NT_STATUS_OK, params, 2, *ppdata, data_return_size,
			    max_data_bytes);

	return;
}