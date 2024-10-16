static void call_trans2open(connection_struct *conn,
			    struct smb_request *req,
			    char **pparams, int total_params,
			    char **ppdata, int total_data,
			    unsigned int max_data_bytes)
{
	struct smb_filename *smb_fname = NULL;
	char *params = *pparams;
	char *pdata = *ppdata;
	int deny_mode;
	int32_t open_attr;
	bool oplock_request;
#if 0
	bool return_additional_info;
	int16 open_sattr;
	time_t open_time;
#endif
	int open_ofun;
	uint32_t open_size;
	char *pname;
	char *fname = NULL;
	off_t size=0;
	int fattr=0,mtime=0;
	SMB_INO_T inode = 0;
	int smb_action = 0;
	files_struct *fsp;
	struct ea_list *ea_list = NULL;
	uint16_t flags = 0;
	NTSTATUS status;
	uint32_t access_mask;
	uint32_t share_mode;
	uint32_t create_disposition;
	uint32_t create_options = 0;
	uint32_t private_flags = 0;
	uint32_t ucf_flags = ucf_flags_from_smb_request(req);
	TALLOC_CTX *ctx = talloc_tos();

	/*
	 * Ensure we have enough parameters to perform the operation.
	 */

	if (total_params < 29) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		goto out;
	}

	flags = SVAL(params, 0);
	deny_mode = SVAL(params, 2);
	open_attr = SVAL(params,6);
        oplock_request = (flags & REQUEST_OPLOCK) ? EXCLUSIVE_OPLOCK : 0;
        if (oplock_request) {
                oplock_request |= (flags & REQUEST_BATCH_OPLOCK) ? BATCH_OPLOCK : 0;
        }

#if 0
	return_additional_info = BITSETW(params,0);
	open_sattr = SVAL(params, 4);
	open_time = make_unix_date3(params+8);
#endif
	open_ofun = SVAL(params,12);
	open_size = IVAL(params,14);
	pname = &params[28];

	if (IS_IPC(conn)) {
		reply_nterror(req, NT_STATUS_NETWORK_ACCESS_DENIED);
		goto out;
	}

	if (req->posix_pathnames) {
		srvstr_get_path_posix(ctx,
			params,
			req->flags2,
			&fname,
			pname,
			total_params - 28,
			STR_TERMINATE,
			&status);
	} else {
		srvstr_get_path(ctx,
			params,
			req->flags2,
			&fname,
			pname,
			total_params - 28,
			STR_TERMINATE,
			&status);
	}
	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		goto out;
	}

	DEBUG(3,("call_trans2open %s deny_mode=0x%x attr=%d ofun=0x%x size=%d\n",
		fname, (unsigned int)deny_mode, (unsigned int)open_attr,
		(unsigned int)open_ofun, open_size));

	status = filename_convert(ctx,
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
			goto out;
		}
		reply_nterror(req, status);
		goto out;
	}

	if (open_ofun == 0) {
		reply_nterror(req, NT_STATUS_OBJECT_NAME_COLLISION);
		goto out;
	}

	if (!map_open_params_to_ntcreate(smb_fname->base_name, deny_mode,
					 open_ofun,
					 &access_mask, &share_mode,
					 &create_disposition,
					 &create_options,
					 &private_flags)) {
		reply_nterror(req, NT_STATUS_ACCESS_DENIED);
		goto out;
	}

	/* Any data in this call is an EA list. */
	if (total_data && (total_data != 4)) {
		if (total_data < 10) {
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			goto out;
		}

		if (IVAL(pdata,0) > total_data) {
			DEBUG(10,("call_trans2open: bad total data size (%u) > %u\n",
				IVAL(pdata,0), (unsigned int)total_data));
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			goto out;
		}

		ea_list = read_ea_list(talloc_tos(), pdata + 4,
				       total_data - 4);
		if (!ea_list) {
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			goto out;
		}

		if (!lp_ea_support(SNUM(conn))) {
			reply_nterror(req, NT_STATUS_EAS_NOT_SUPPORTED);
			goto out;
		}

		if (!req->posix_pathnames &&
				ea_list_has_invalid_name(ea_list)) {
			int param_len = 30;
			*pparams = (char *)SMB_REALLOC(*pparams, param_len);
			if(*pparams == NULL ) {
				reply_nterror(req, NT_STATUS_NO_MEMORY);
				goto out;
			}
			params = *pparams;
			memset(params, '\0', param_len);
			send_trans2_replies(conn, req, STATUS_INVALID_EA_NAME,
				params, param_len, NULL, 0, max_data_bytes);
			goto out;
		}
	}

	status = SMB_VFS_CREATE_FILE(
		conn,					/* conn */
		req,					/* req */
		smb_fname,				/* fname */
		access_mask,				/* access_mask */
		share_mode,				/* share_access */
		create_disposition,			/* create_disposition*/
		create_options,				/* create_options */
		open_attr,				/* file_attributes */
		oplock_request,				/* oplock_request */
		NULL,					/* lease */
		open_size,				/* allocation_size */
		private_flags,
		NULL,					/* sd */
		ea_list,				/* ea_list */
		&fsp,					/* result */
		&smb_action,				/* psbuf */
		NULL, NULL);				/* create context */

	if (!NT_STATUS_IS_OK(status)) {
		if (open_was_deferred(req->xconn, req->mid)) {
			/* We have re-scheduled this call. */
			goto out;
		}

		if (!NT_STATUS_EQUAL(status, NT_STATUS_SHARING_VIOLATION)) {
			reply_openerror(req, status);
			goto out;
		}

		fsp = fcb_or_dos_open(
			req,
			smb_fname,
			access_mask,
			create_options,
			private_flags);
		if (fsp == NULL) {
			bool ok = defer_smb1_sharing_violation(req);
			if (ok) {
				goto out;
			}
			reply_openerror(req, status);
			goto out;
		}

		smb_action = FILE_WAS_OPENED;
	}

	size = get_file_size_stat(&smb_fname->st);
	fattr = fdos_mode(fsp);
	mtime = convert_timespec_to_time_t(smb_fname->st.st_ex_mtime);
	inode = smb_fname->st.st_ex_ino;
	if (fattr & FILE_ATTRIBUTE_DIRECTORY) {
		close_file(req, fsp, ERROR_CLOSE);
		reply_nterror(req, NT_STATUS_ACCESS_DENIED);
		goto out;
	}

	/* Realloc the size of parameters and data we will return */
	*pparams = (char *)SMB_REALLOC(*pparams, 30);
	if(*pparams == NULL ) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		goto out;
	}
	params = *pparams;

	SSVAL(params,0,fsp->fnum);
	SSVAL(params,2,fattr);
	srv_put_dos_date2(params,4, mtime);
	SIVAL(params,8, (uint32_t)size);
	SSVAL(params,12,deny_mode);
	SSVAL(params,14,0); /* open_type - file or directory. */
	SSVAL(params,16,0); /* open_state - only valid for IPC device. */

	if (oplock_request && lp_fake_oplocks(SNUM(conn))) {
		smb_action |= EXTENDED_OPLOCK_GRANTED;
	}

	SSVAL(params,18,smb_action);

	/*
	 * WARNING - this may need to be changed if SMB_INO_T <> 4 bytes.
	 */
	SIVAL(params,20,inode);
	SSVAL(params,24,0); /* Padding. */
	if (flags & 8) {
		uint32_t ea_size = estimate_ea_size(smb_fname->fsp);
		SIVAL(params, 26, ea_size);
	} else {
		SIVAL(params, 26, 0);
	}

	/* Send the required number of replies */
	send_trans2_replies(conn, req, NT_STATUS_OK, params, 30, *ppdata, 0, max_data_bytes);
 out:
	TALLOC_FREE(smb_fname);
}