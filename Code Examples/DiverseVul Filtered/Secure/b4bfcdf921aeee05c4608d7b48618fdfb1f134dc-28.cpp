void reply_ntrename(struct smb_request *req)
{
	connection_struct *conn = req->conn;
	struct smb_filename *smb_fname_old = NULL;
	struct smb_filename *smb_fname_new = NULL;
	char *oldname = NULL;
	char *newname = NULL;
	const char *p;
	NTSTATUS status;
	bool src_has_wcard = False;
	bool dest_has_wcard = False;
	uint32 attrs;
	uint32_t ucf_flags_src = 0;
	uint32_t ucf_flags_dst = 0;
	uint16 rename_type;
	TALLOC_CTX *ctx = talloc_tos();
	bool stream_rename = false;

	START_PROFILE(SMBntrename);

	if (req->wct < 4) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		goto out;
	}

	attrs = SVAL(req->vwv+0, 0);
	rename_type = SVAL(req->vwv+1, 0);

	p = (const char *)req->buf + 1;
	p += srvstr_get_path_req_wcard(ctx, req, &oldname, p, STR_TERMINATE,
				       &status, &src_has_wcard);
	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		goto out;
	}

	if (ms_has_wild(oldname)) {
		reply_nterror(req, NT_STATUS_OBJECT_PATH_SYNTAX_BAD);
		goto out;
	}

	p++;
	p += srvstr_get_path_req_wcard(ctx, req, &newname, p, STR_TERMINATE,
				       &status, &dest_has_wcard);
	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		goto out;
	}

	if (!lp_posix_pathnames()) {
		/* The newname must begin with a ':' if the
		   oldname contains a ':'. */
		if (strchr_m(oldname, ':')) {
			if (newname[0] != ':') {
				reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
				goto out;
			}
			stream_rename = true;
		}
	}

	/*
	 * If this is a rename operation, allow wildcards and save the
	 * destination's last component.
	 */
	if (rename_type == RENAME_FLAG_RENAME) {
		ucf_flags_src = UCF_COND_ALLOW_WCARD_LCOMP;
		ucf_flags_dst = UCF_COND_ALLOW_WCARD_LCOMP | UCF_SAVE_LCOMP;
	}

	/* rename_internals() calls unix_convert(), so don't call it here. */
	status = filename_convert(ctx, conn,
				  req->flags2 & FLAGS2_DFS_PATHNAMES,
				  oldname,
				  ucf_flags_src,
				  NULL,
				  &smb_fname_old);
	if (!NT_STATUS_IS_OK(status)) {
		if (NT_STATUS_EQUAL(status,
				    NT_STATUS_PATH_NOT_COVERED)) {
			reply_botherror(req,
					NT_STATUS_PATH_NOT_COVERED,
					ERRSRV, ERRbadpath);
			goto out;
		}
		reply_nterror(req, status);
		goto out;
	}

	status = filename_convert(ctx, conn,
				  req->flags2 & FLAGS2_DFS_PATHNAMES,
				  newname,
				  ucf_flags_dst,
				  &dest_has_wcard,
				  &smb_fname_new);
	if (!NT_STATUS_IS_OK(status)) {
		if (NT_STATUS_EQUAL(status,
				    NT_STATUS_PATH_NOT_COVERED)) {
			reply_botherror(req,
				        NT_STATUS_PATH_NOT_COVERED,
					ERRSRV, ERRbadpath);
			goto out;
		}
		reply_nterror(req, status);
		goto out;
	}

	if (stream_rename) {
		/* smb_fname_new must be the same as smb_fname_old. */
		TALLOC_FREE(smb_fname_new->base_name);
		smb_fname_new->base_name = talloc_strdup(smb_fname_new,
						smb_fname_old->base_name);
		if (!smb_fname_new->base_name) {
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			goto out;
		}
	}

	DEBUG(3,("reply_ntrename: %s -> %s\n",
		 smb_fname_str_dbg(smb_fname_old),
		 smb_fname_str_dbg(smb_fname_new)));

	switch(rename_type) {
		case RENAME_FLAG_RENAME:
			status = rename_internals(ctx, conn, req,
						  smb_fname_old, smb_fname_new,
						  attrs, False, src_has_wcard,
						  dest_has_wcard,
						  DELETE_ACCESS);
			break;
		case RENAME_FLAG_HARD_LINK:
			if (src_has_wcard || dest_has_wcard) {
				/* No wildcards. */
				status = NT_STATUS_OBJECT_PATH_SYNTAX_BAD;
			} else {
				status = hardlink_internals(ctx, conn,
							    req,
							    false,
							    smb_fname_old,
							    smb_fname_new);
			}
			break;
		case RENAME_FLAG_COPY:
			if (src_has_wcard || dest_has_wcard) {
				/* No wildcards. */
				status = NT_STATUS_OBJECT_PATH_SYNTAX_BAD;
			} else {
				status = copy_internals(ctx, conn, req,
							smb_fname_old,
							smb_fname_new,
							attrs);
			}
			break;
		case RENAME_FLAG_MOVE_CLUSTER_INFORMATION:
			status = NT_STATUS_INVALID_PARAMETER;
			break;
		default:
			status = NT_STATUS_ACCESS_DENIED; /* Default error. */
			break;
	}

	if (!NT_STATUS_IS_OK(status)) {
		if (open_was_deferred(req->sconn, req->mid)) {
			/* We have re-scheduled this call. */
			goto out;
		}

		reply_nterror(req, status);
		goto out;
	}

	reply_outbuf(req, 0, 0);
 out:
	END_PROFILE(SMBntrename);
	return;
}