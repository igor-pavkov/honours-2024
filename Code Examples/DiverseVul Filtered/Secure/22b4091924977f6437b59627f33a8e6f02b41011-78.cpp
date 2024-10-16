static void call_trans2findnext(connection_struct *conn,
				struct smb_request *req,
				char **pparams, int total_params,
				char **ppdata, int total_data,
				unsigned int max_data_bytes)
{
	/* We must be careful here that we don't return more than the
		allowed number of data bytes. If this means returning fewer than
		maxentries then so be it. We assume that the redirector has
		enough room for the fixed number of parameter bytes it has
		requested. */
	char *params = *pparams;
	char *pdata = *ppdata;
	char *data_end;
	int dptr_num;
	int maxentries;
	uint16_t info_level;
	uint32_t resume_key;
	uint16_t findnext_flags;
	bool close_after_request;
	bool close_if_end;
	bool requires_resume_key;
	bool continue_bit;
	char *resume_name = NULL;
	const char *mask = NULL;
	const char *directory = NULL;
	char *p = NULL;
	uint16_t dirtype;
	int numentries = 0;
	int i, last_entry_off=0;
	bool finished = False;
	bool dont_descend = False;
	bool out_of_space = False;
	int space_remaining;
	struct ea_list *ea_list = NULL;
	NTSTATUS ntstatus = NT_STATUS_OK;
	bool ask_sharemode = lp_smbd_search_ask_sharemode(SNUM(conn));
	TALLOC_CTX *ctx = talloc_tos();
	struct smbd_server_connection *sconn = req->sconn;
	bool backup_priv = false; 
	bool as_root = false;
	files_struct *fsp = NULL;
	const struct loadparm_substitution *lp_sub =
		loadparm_s3_global_substitution();

	if (total_params < 13) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	dptr_num = SVAL(params,0);
	maxentries = SVAL(params,2);
	info_level = SVAL(params,4);
	resume_key = IVAL(params,6);
	findnext_flags = SVAL(params,10);
	close_after_request = (findnext_flags & FLAG_TRANS2_FIND_CLOSE);
	close_if_end = (findnext_flags & FLAG_TRANS2_FIND_CLOSE_IF_END);
	requires_resume_key = (findnext_flags & FLAG_TRANS2_FIND_REQUIRE_RESUME);
	continue_bit = (findnext_flags & FLAG_TRANS2_FIND_CONTINUE);

	if (!continue_bit) {
		/* We only need resume_name if continue_bit is zero. */
		if (req->posix_pathnames) {
			srvstr_get_path_posix(ctx,
				params,
				req->flags2,
				&resume_name,
				params+12,
				total_params - 12,
				STR_TERMINATE,
				&ntstatus);
		} else {
			srvstr_get_path(ctx,
				params,
				req->flags2,
				&resume_name,
				params+12,
				total_params - 12,
				STR_TERMINATE,
				&ntstatus);
		}
		if (!NT_STATUS_IS_OK(ntstatus)) {
			/* Win9x or OS/2 can send a resume name of ".." or ".". This will cause the parser to
			   complain (it thinks we're asking for the directory above the shared
			   path or an invalid name). Catch this as the resume name is only compared, never used in
			   a file access. JRA. */
			srvstr_pull_talloc(ctx, params, req->flags2,
				&resume_name, params+12,
				total_params - 12,
				STR_TERMINATE);

			if (!resume_name || !(ISDOT(resume_name) || ISDOTDOT(resume_name))) {
				reply_nterror(req, ntstatus);
				return;
			}
		}
	}

	DEBUG(3,("call_trans2findnext: dirhandle = %d, max_data_bytes = %d, maxentries = %d, \
close_after_request=%d, close_if_end = %d requires_resume_key = %d \
resume_key = %d resume name = %s continue=%d level = %d\n",
		dptr_num, max_data_bytes, maxentries, close_after_request, close_if_end, 
		requires_resume_key, resume_key,
		resume_name ? resume_name : "(NULL)", continue_bit, info_level));

	if (!maxentries) {
		/* W2K3 seems to treat zero as 1. */
		maxentries = 1;
	}

	switch (info_level) {
		case SMB_FIND_INFO_STANDARD:
		case SMB_FIND_EA_SIZE:
		case SMB_FIND_EA_LIST:
		case SMB_FIND_FILE_DIRECTORY_INFO:
		case SMB_FIND_FILE_FULL_DIRECTORY_INFO:
		case SMB_FIND_FILE_NAMES_INFO:
		case SMB_FIND_FILE_BOTH_DIRECTORY_INFO:
		case SMB_FIND_ID_FULL_DIRECTORY_INFO:
		case SMB_FIND_ID_BOTH_DIRECTORY_INFO:
			break;
		case SMB_FIND_FILE_UNIX:
		case SMB_FIND_FILE_UNIX_INFO2:
			/* Always use filesystem for UNIX mtime query. */
			ask_sharemode = false;
			if (!lp_unix_extensions()) {
				reply_nterror(req, NT_STATUS_INVALID_LEVEL);
				return;
			}
			break;
		default:
			reply_nterror(req, NT_STATUS_INVALID_LEVEL);
			return;
	}

	if (info_level == SMB_FIND_EA_LIST) {
		uint32_t ea_size;

		if (total_data < 4) {
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			return;
		}

		ea_size = IVAL(pdata,0);
		if (ea_size != total_data) {
			DEBUG(4,("call_trans2findnext: Rejecting EA request with incorrect \
total_data=%u (should be %u)\n", (unsigned int)total_data, (unsigned int)IVAL(pdata,0) ));
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			return;
		}

		if (!lp_ea_support(SNUM(conn))) {
			reply_nterror(req, NT_STATUS_EAS_NOT_SUPPORTED);
			return;
		}

		/* Pull out the list of names. */
		ea_list = read_ea_name_list(ctx, pdata + 4, ea_size - 4);
		if (!ea_list) {
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			return;
		}
	}

	if (max_data_bytes + DIR_ENTRY_SAFETY_MARGIN < max_data_bytes) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	*ppdata = (char *)SMB_REALLOC(
		*ppdata, max_data_bytes + DIR_ENTRY_SAFETY_MARGIN);
	if(*ppdata == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		return;
	}

	pdata = *ppdata;
	data_end = pdata + max_data_bytes + DIR_ENTRY_SAFETY_MARGIN - 1;

	/*
	 * squash valgrind "writev(vector[...]) points to uninitialised byte(s)"
	 * error.
	 */
	memset(pdata + total_data, 0, (max_data_bytes + DIR_ENTRY_SAFETY_MARGIN) - total_data);
	/* Realloc the params space */
	*pparams = (char *)SMB_REALLOC(*pparams, 6*SIZEOFWORD);
	if(*pparams == NULL ) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		return;
	}

	params = *pparams;

	/* Check that the dptr is valid */
	fsp = dptr_fetch_lanman2_fsp(sconn, dptr_num);
	if (fsp == NULL) {
		reply_nterror(req, STATUS_NO_MORE_FILES);
		return;
	}

	directory = dptr_path(sconn, dptr_num);

	/* Get the wildcard mask from the dptr */
	if((mask = dptr_wcard(sconn, dptr_num))== NULL) {
		DEBUG(2,("dptr_num %d has no wildcard\n", dptr_num));
		reply_nterror(req, STATUS_NO_MORE_FILES);
		return;
	}

	/* Get the attr mask from the dptr */
	dirtype = dptr_attr(sconn, dptr_num);

	backup_priv = dptr_get_priv(fsp->dptr);

	DEBUG(3,("dptr_num is %d, mask = %s, attr = %x, dirptr=(0x%lX,%ld) "
		"backup_priv = %d\n",
		dptr_num, mask, dirtype,
		(long)fsp->dptr,
		dptr_TellDir(fsp->dptr),
		(int)backup_priv));

	/* We don't need to check for VOL here as this is returned by
		a different TRANS2 call. */

	DEBUG(8,("dirpath=<%s> dontdescend=<%s>\n",
		 directory,lp_dont_descend(ctx, lp_sub, SNUM(conn))));
	if (in_list(directory,lp_dont_descend(ctx, lp_sub, SNUM(conn)),conn->case_sensitive))
		dont_descend = True;

	p = pdata;
	space_remaining = max_data_bytes;
	out_of_space = False;

	if (backup_priv) {
		become_root();
		as_root = true;
	}

	/*
	 * Seek to the correct position. We no longer use the resume key but
	 * depend on the last file name instead.
	 */

	if(!continue_bit && resume_name && *resume_name) {
		SMB_STRUCT_STAT st;

		long current_pos = 0;
		/*
		 * Remember, name_to_8_3 is called by
		 * get_lanman2_dir_entry(), so the resume name
		 * could be mangled. Ensure we check the unmangled name.
		 */

		if (mangle_is_mangled(resume_name, conn->params)) {
			char *new_resume_name = NULL;
			mangle_lookup_name_from_8_3(ctx,
						resume_name,
						&new_resume_name,
						conn->params);
			if (new_resume_name) {
				resume_name = new_resume_name;
			}
		}

		/*
		 * Fix for NT redirector problem triggered by resume key indexes
		 * changing between directory scans. We now return a resume key of 0
		 * and instead look for the filename to continue from (also given
		 * to us by NT/95/smbfs/smbclient). If no other scans have been done between the
		 * findfirst/findnext (as is usual) then the directory pointer
		 * should already be at the correct place.
		 */

		finished = !dptr_SearchDir(fsp->dptr, resume_name, &current_pos, &st);
	} /* end if resume_name && !continue_bit */

	for (i=0;(i<(int)maxentries) && !finished && !out_of_space ;i++) {
		bool got_exact_match = False;

		/* this is a heuristic to avoid seeking the fsp->dptr except when
			absolutely necessary. It allows for a filename of about 40 chars */
		if (space_remaining < DIRLEN_GUESS && numentries > 0) {
			out_of_space = True;
			finished = False;
		} else {
			ntstatus = get_lanman2_dir_entry(ctx,
						conn,
						fsp->dptr,
						req->flags2,
						mask,dirtype,info_level,
						requires_resume_key,dont_descend,
						ask_sharemode,
						&p,pdata,data_end,
						space_remaining,
						&got_exact_match,
						&last_entry_off, ea_list);
			if (NT_STATUS_EQUAL(ntstatus,
					NT_STATUS_ILLEGAL_CHARACTER)) {
				/*
				 * Bad character conversion on name. Ignore this
				 * entry.
				 */
				continue;
			}
			if (NT_STATUS_EQUAL(ntstatus, STATUS_MORE_ENTRIES)) {
				out_of_space = true;
			} else {
				finished = !NT_STATUS_IS_OK(ntstatus);
			}
		}

		if (!finished && !out_of_space)
			numentries++;

		/*
		 * As an optimisation if we know we aren't looking
		 * for a wildcard name (ie. the name matches the wildcard exactly)
		 * then we can finish on any (first) match.
		 * This speeds up large directory searches. JRA.
		 */

		if(got_exact_match)
			finished = True;

		space_remaining = max_data_bytes - PTR_DIFF(p,pdata);
	}

	DEBUG( 3, ( "%s mask=%s directory=%s dirtype=%d numentries=%d\n",
		smb_fn_name(req->cmd),
		mask, directory, dirtype, numentries ) );

	/* Check if we can close the fsp->dptr */
	if(close_after_request || (finished && close_if_end)) {
		DEBUG(5,("call_trans2findnext: closing dptr_num = %d\n", dptr_num));
		dptr_num = -1;
		close_file(NULL, fsp, NORMAL_CLOSE);
		fsp = NULL;
	}

	if (as_root) {
		unbecome_root();
	}

	/* Set up the return parameter block */
	SSVAL(params,0,numentries);
	SSVAL(params,2,finished);
	SSVAL(params,4,0); /* Never an EA error */
	SSVAL(params,6,last_entry_off);

	send_trans2_replies(conn, req, NT_STATUS_OK, params, 8, pdata, PTR_DIFF(p,pdata),
			    max_data_bytes);

	return;
}