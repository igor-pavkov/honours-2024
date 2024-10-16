void reply_ntcreate_and_X(struct smb_request *req)
{
	connection_struct *conn = req->conn;
	struct smb_filename *smb_fname = NULL;
	char *fname = NULL;
	uint32 flags;
	uint32 access_mask;
	uint32 file_attributes;
	uint32 share_access;
	uint32 create_disposition;
	uint32 create_options;
	uint16 root_dir_fid;
	uint64_t allocation_size;
	/* Breakout the oplock request bits so we can set the
	   reply bits separately. */
	uint32 fattr=0;
	off_t file_len = 0;
	int info = 0;
	files_struct *fsp = NULL;
	char *p = NULL;
	struct timespec create_timespec;
	struct timespec c_timespec;
	struct timespec a_timespec;
	struct timespec m_timespec;
	struct timespec write_time_ts;
	NTSTATUS status;
	int oplock_request;
	uint8_t oplock_granted = NO_OPLOCK_RETURN;
	struct case_semantics_state *case_state = NULL;
	TALLOC_CTX *ctx = talloc_tos();

	START_PROFILE(SMBntcreateX);

	if (req->wct < 24) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		goto out;
	}

	flags = IVAL(req->vwv+3, 1);
	access_mask = IVAL(req->vwv+7, 1);
	file_attributes = IVAL(req->vwv+13, 1);
	share_access = IVAL(req->vwv+15, 1);
	create_disposition = IVAL(req->vwv+17, 1);
	create_options = IVAL(req->vwv+19, 1);
	root_dir_fid = (uint16)IVAL(req->vwv+5, 1);

	allocation_size = BVAL(req->vwv+9, 1);

	srvstr_get_path_req(ctx, req, &fname, (const char *)req->buf,
			    STR_TERMINATE, &status);

	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		goto out;
	}

	DEBUG(10,("reply_ntcreate_and_X: flags = 0x%x, access_mask = 0x%x "
		  "file_attributes = 0x%x, share_access = 0x%x, "
		  "create_disposition = 0x%x create_options = 0x%x "
		  "root_dir_fid = 0x%x, fname = %s\n",
			(unsigned int)flags,
			(unsigned int)access_mask,
			(unsigned int)file_attributes,
			(unsigned int)share_access,
			(unsigned int)create_disposition,
			(unsigned int)create_options,
			(unsigned int)root_dir_fid,
			fname));

	/*
	 * we need to remove ignored bits when they come directly from the client
	 * because we reuse some of them for internal stuff
	 */
	create_options &= ~NTCREATEX_OPTIONS_MUST_IGNORE_MASK;

	/*
	 * If it's an IPC, use the pipe handler.
	 */

	if (IS_IPC(conn)) {
		if (lp_nt_pipe_support()) {
			do_ntcreate_pipe_open(conn, req);
			goto out;
		}
		reply_nterror(req, NT_STATUS_ACCESS_DENIED);
		goto out;
	}

	oplock_request = (flags & REQUEST_OPLOCK) ? EXCLUSIVE_OPLOCK : 0;
	if (oplock_request) {
		oplock_request |= (flags & REQUEST_BATCH_OPLOCK)
			? BATCH_OPLOCK : 0;
	}

	if (file_attributes & FILE_FLAG_POSIX_SEMANTICS) {
		case_state = set_posix_case_semantics(ctx, conn);
		if (!case_state) {
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			goto out;
		}
	}

	status = filename_convert(ctx,
				conn,
				req->flags2 & FLAGS2_DFS_PATHNAMES,
				fname,
				0,
				NULL,
				&smb_fname);

	TALLOC_FREE(case_state);

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

	/*
	 * Bug #6898 - clients using Windows opens should
	 * never be able to set this attribute into the
	 * VFS.
	 */
	file_attributes &= ~FILE_FLAG_POSIX_SEMANTICS;

	status = SMB_VFS_CREATE_FILE(
		conn,					/* conn */
		req,					/* req */
		root_dir_fid,				/* root_dir_fid */
		smb_fname,				/* fname */
		access_mask,				/* access_mask */
		share_access,				/* share_access */
		create_disposition,			/* create_disposition*/
		create_options,				/* create_options */
		file_attributes,			/* file_attributes */
		oplock_request,				/* oplock_request */
		allocation_size,			/* allocation_size */
		0,					/* private_flags */
		NULL,					/* sd */
		NULL,					/* ea_list */
		&fsp,					/* result */
		&info);					/* pinfo */

	if (!NT_STATUS_IS_OK(status)) {
		if (open_was_deferred(req->sconn, req->mid)) {
			/* We have re-scheduled this call, no error. */
			goto out;
		}
		reply_openerror(req, status);
		goto out;
	}

	/* Ensure we're pointing at the correct stat struct. */
	TALLOC_FREE(smb_fname);
	smb_fname = fsp->fsp_name;

	/*
	 * If the caller set the extended oplock request bit
	 * and we granted one (by whatever means) - set the
	 * correct bit for extended oplock reply.
	 */

	if (oplock_request &&
	    (lp_fake_oplocks(SNUM(conn))
	     || EXCLUSIVE_OPLOCK_TYPE(fsp->oplock_type))) {

		/*
		 * Exclusive oplock granted
		 */

		if (flags & REQUEST_BATCH_OPLOCK) {
			oplock_granted = BATCH_OPLOCK_RETURN;
		} else {
			oplock_granted = EXCLUSIVE_OPLOCK_RETURN;
		}
	} else if (fsp->oplock_type == LEVEL_II_OPLOCK) {
		oplock_granted = LEVEL_II_OPLOCK_RETURN;
	} else {
		oplock_granted = NO_OPLOCK_RETURN;
	}

	file_len = smb_fname->st.st_ex_size;

	if (flags & EXTENDED_RESPONSE_REQUIRED) {
		/* This is very strange. We
 		 * return 50 words, but only set
		 * the wcnt to 42 ? It's definitely
 		 * what happens on the wire....
 		 */
		reply_outbuf(req, 50, 0);
		SCVAL(req->outbuf,smb_wct,42);
	} else {
		reply_outbuf(req, 34, 0);
	}

	SSVAL(req->outbuf, smb_vwv0, 0xff); /* andx chain ends */
	SSVAL(req->outbuf, smb_vwv1, 0);    /* no andx offset */

	p = (char *)req->outbuf + smb_vwv2;

	SCVAL(p, 0, oplock_granted);

	p++;
	SSVAL(p,0,fsp->fnum);
	p += 2;
	if ((create_disposition == FILE_SUPERSEDE)
	    && (info == FILE_WAS_OVERWRITTEN)) {
		SIVAL(p,0,FILE_WAS_SUPERSEDED);
	} else {
		SIVAL(p,0,info);
	}
	p += 4;

	fattr = dos_mode(conn, smb_fname);
	if (fattr == 0) {
		fattr = FILE_ATTRIBUTE_NORMAL;
	}

	/* Deal with other possible opens having a modified
	   write time. JRA. */
	ZERO_STRUCT(write_time_ts);
	get_file_infos(fsp->file_id, 0, NULL, &write_time_ts);
	if (!null_timespec(write_time_ts)) {
		update_stat_ex_mtime(&smb_fname->st, write_time_ts);
	}

	/* Create time. */
	create_timespec = get_create_timespec(conn, fsp, smb_fname);
	a_timespec = smb_fname->st.st_ex_atime;
	m_timespec = smb_fname->st.st_ex_mtime;
	c_timespec = get_change_timespec(conn, fsp, smb_fname);

	if (lp_dos_filetime_resolution(SNUM(conn))) {
		dos_filetime_timespec(&create_timespec);
		dos_filetime_timespec(&a_timespec);
		dos_filetime_timespec(&m_timespec);
		dos_filetime_timespec(&c_timespec);
	}

	put_long_date_timespec(conn->ts_res, p, create_timespec); /* create time. */
	p += 8;
	put_long_date_timespec(conn->ts_res, p, a_timespec); /* access time */
	p += 8;
	put_long_date_timespec(conn->ts_res, p, m_timespec); /* write time */
	p += 8;
	put_long_date_timespec(conn->ts_res, p, c_timespec); /* change time */
	p += 8;
	SIVAL(p,0,fattr); /* File Attributes. */
	p += 4;
	SOFF_T(p, 0, SMB_VFS_GET_ALLOC_SIZE(conn,fsp,&smb_fname->st));
	p += 8;
	SOFF_T(p,0,file_len);
	p += 8;
	if (flags & EXTENDED_RESPONSE_REQUIRED) {
		uint16_t file_status = (NO_EAS|NO_SUBSTREAMS|NO_REPARSETAG);
		size_t num_names = 0;
		unsigned int num_streams = 0;
		struct stream_struct *streams = NULL;

		/* Do we have any EA's ? */
		status = get_ea_names_from_file(ctx, conn, fsp,
				smb_fname->base_name, NULL, &num_names);
		if (NT_STATUS_IS_OK(status) && num_names) {
			file_status &= ~NO_EAS;
		}
		status = vfs_streaminfo(conn, NULL, smb_fname->base_name, ctx,
			&num_streams, &streams);
		/* There is always one stream, ::$DATA. */
		if (NT_STATUS_IS_OK(status) && num_streams > 1) {
			file_status &= ~NO_SUBSTREAMS;
		}
		TALLOC_FREE(streams);
		SSVAL(p,2,file_status);
	}
	p += 4;
	SCVAL(p,0,fsp->is_directory ? 1 : 0);

	if (flags & EXTENDED_RESPONSE_REQUIRED) {
		uint32 perms = 0;
		p += 25;
		if (fsp->is_directory ||
		    can_write_to_file(conn, smb_fname)) {
			perms = FILE_GENERIC_ALL;
		} else {
			perms = FILE_GENERIC_READ|FILE_EXECUTE;
		}
		SIVAL(p,0,perms);
	}

	DEBUG(5,("reply_ntcreate_and_X: %s, open name = %s\n",
		fsp_fnum_dbg(fsp), smb_fname_str_dbg(smb_fname)));

 out:
	END_PROFILE(SMBntcreateX);
	return;
}