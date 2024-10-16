static void call_trans2findfirst(connection_struct *conn,
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
	struct smb_filename *smb_dname = NULL;
	char *params = *pparams;
	char *pdata = *ppdata;
	char *data_end;
	uint32_t dirtype;
	int maxentries;
	uint16_t findfirst_flags;
	bool close_after_first;
	bool close_if_end;
	bool requires_resume_key;
	int info_level;
	char *directory = NULL;
	char *mask = NULL;
	char *p;
	int last_entry_off=0;
	int dptr_num = -1;
	int numentries = 0;
	int i;
	bool finished = False;
	bool dont_descend = False;
	bool out_of_space = False;
	int space_remaining;
	struct ea_list *ea_list = NULL;
	NTSTATUS ntstatus = NT_STATUS_OK;
	bool ask_sharemode = lp_smbd_search_ask_sharemode(SNUM(conn));
	struct smbd_server_connection *sconn = req->sconn;
	uint32_t ucf_flags = UCF_ALWAYS_ALLOW_WCARD_LCOMP |
			ucf_flags_from_smb_request(req);
	bool backup_priv = false;
	bool as_root = false;
	files_struct *fsp = NULL;
	const struct loadparm_substitution *lp_sub =
		loadparm_s3_global_substitution();
	int ret;

	if (total_params < 13) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		goto out;
	}

	dirtype = SVAL(params,0);
	maxentries = SVAL(params,2);
	findfirst_flags = SVAL(params,4);
	close_after_first = (findfirst_flags & FLAG_TRANS2_FIND_CLOSE);
	close_if_end = (findfirst_flags & FLAG_TRANS2_FIND_CLOSE_IF_END);
	requires_resume_key = (findfirst_flags & FLAG_TRANS2_FIND_REQUIRE_RESUME);
	backup_priv = ((findfirst_flags & FLAG_TRANS2_FIND_BACKUP_INTENT) &&
				security_token_has_privilege(get_current_nttok(conn),
						SEC_PRIV_BACKUP));

	info_level = SVAL(params,6);

	DEBUG(3,("call_trans2findfirst: dirtype = %x, maxentries = %d, close_after_first=%d, \
close_if_end = %d requires_resume_key = %d backup_priv = %d level = 0x%x, max_data_bytes = %d\n",
		(unsigned int)dirtype, maxentries, close_after_first, close_if_end, requires_resume_key,
		(int)backup_priv,
		info_level, max_data_bytes));

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
			if (!lp_unix_extensions()) {
				reply_nterror(req, NT_STATUS_INVALID_LEVEL);
				goto out;
			}
			break;
		default:
			reply_nterror(req, NT_STATUS_INVALID_LEVEL);
			goto out;
	}

	if (req->posix_pathnames) {
		/* Always use filesystem for UNIX mtime query. */
		ask_sharemode = false;
	}

	if (req->posix_pathnames) {
		srvstr_get_path_posix(talloc_tos(),
				params,
				req->flags2,
				&directory,
				params+12,
				total_params - 12,
				STR_TERMINATE,
				&ntstatus);
	} else {
		srvstr_get_path(talloc_tos(),
				params,
				req->flags2,
				&directory,
				params+12,
				total_params - 12,
				STR_TERMINATE,
				&ntstatus);
	}
	if (!NT_STATUS_IS_OK(ntstatus)) {
		reply_nterror(req, ntstatus);
		goto out;
	}

	if (backup_priv) {
		become_root();
		as_root = true;
		ntstatus = filename_convert_with_privilege(talloc_tos(),
				conn,
				req,
				directory,
				ucf_flags,
				&smb_dname);
	} else {
		ntstatus = filename_convert(talloc_tos(), conn,
				    directory,
				    ucf_flags,
				    0,
				    &smb_dname);
	}

	if (!NT_STATUS_IS_OK(ntstatus)) {
		if (NT_STATUS_EQUAL(ntstatus,NT_STATUS_PATH_NOT_COVERED)) {
			reply_botherror(req, NT_STATUS_PATH_NOT_COVERED,
					ERRSRV, ERRbadpath);
			goto out;
		}
		reply_nterror(req, ntstatus);
		goto out;
	}

	/*
	 * The above call to filename_convert() is on the path from the client
	 * including the search mask. Until the code that chops of the search
	 * mask from the path below is moved before the call to
	 * filename_convert(), we close a possible pathref fsp to ensure
	 * SMB_VFS_CREATE_FILE() below will internally open a pathref fsp on the
	 * correct path.
	 */
	if (smb_dname->fsp != NULL) {
		ntstatus = fd_close(smb_dname->fsp);
		if (!NT_STATUS_IS_OK(ntstatus)) {
			reply_nterror(req, ntstatus);
			goto out;
		}
		/*
		 * The pathref fsp link destructor will set smb_dname->fsp to
		 * NULL. Turning this into an assert to give a hint at readers
		 * of the code trying to understand the mechanics.
		 */
		file_free(req, smb_dname->fsp);
		SMB_ASSERT(smb_dname->fsp == NULL);
	}

	mask = get_original_lcomp(talloc_tos(),
				conn,
				directory,
				ucf_flags);
	if (mask == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		goto out;
	}

	directory = smb_dname->base_name;

	p = strrchr_m(directory,'/');
	if(p == NULL) {
		/* Windows and OS/2 systems treat search on the root '\' as if it were '\*' */
		if((directory[0] == '.') && (directory[1] == '\0')) {
			mask = talloc_strdup(talloc_tos(),"*");
			if (!mask) {
				reply_nterror(req, NT_STATUS_NO_MEMORY);
				goto out;
			}
		}
	} else {
		*p = 0;
	}

	if (p == NULL || p == directory) {
		struct smb_filename *old_name = smb_dname;

		/* Ensure we don't have a directory name of "". */
		smb_dname = synthetic_smb_fname(talloc_tos(),
						".",
						NULL,
						&old_name->st,
						old_name->twrp,
						old_name->flags);
		TALLOC_FREE(old_name);
		if (smb_dname == NULL) {
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			goto out;
		}
		directory = smb_dname->base_name;
	}

	DEBUG(5,("dir=%s, mask = %s\n",directory, mask));

	if (info_level == SMB_FIND_EA_LIST) {
		uint32_t ea_size;

		if (total_data < 4) {
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			goto out;
		}

		ea_size = IVAL(pdata,0);
		if (ea_size != total_data) {
			DEBUG(4,("call_trans2findfirst: Rejecting EA request with incorrect \
total_data=%u (should be %u)\n", (unsigned int)total_data, (unsigned int)IVAL(pdata,0) ));
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			goto out;
		}

		if (!lp_ea_support(SNUM(conn))) {
			reply_nterror(req, NT_STATUS_EAS_NOT_SUPPORTED);
			goto out;
		}

		/* Pull out the list of names. */
		ea_list = read_ea_name_list(talloc_tos(), pdata + 4, ea_size - 4);
		if (!ea_list) {
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			goto out;
		}
	}

	if (max_data_bytes + DIR_ENTRY_SAFETY_MARGIN < max_data_bytes) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		goto out;
	}

	*ppdata = (char *)SMB_REALLOC(
		*ppdata, max_data_bytes + DIR_ENTRY_SAFETY_MARGIN);
	if(*ppdata == NULL ) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		goto out;
	}
	pdata = *ppdata;
	data_end = pdata + max_data_bytes + DIR_ENTRY_SAFETY_MARGIN - 1;
	/*
	 * squash valgrind "writev(vector[...]) points to uninitialised byte(s)"
	 * error.
	 */
	memset(pdata + total_data, 0, ((max_data_bytes + DIR_ENTRY_SAFETY_MARGIN) - total_data));
	/* Realloc the params space */
	*pparams = (char *)SMB_REALLOC(*pparams, 10);
	if (*pparams == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		goto out;
	}
	params = *pparams;

	/*
	 * As we've cut off the last component from
	 * smb_fname we need to re-stat smb_dname
	 * so FILE_OPEN disposition knows the directory
	 * exists.
	 */
	ret = vfs_stat(conn, smb_dname);
	if (ret == -1) {
		ntstatus = map_nt_error_from_unix(errno);
		reply_nterror(req, ntstatus);
		goto out;
	}

	ntstatus = openat_pathref_fsp(conn->cwd_fsp, smb_dname);
	if (!NT_STATUS_IS_OK(ntstatus)) {
		reply_nterror(req, ntstatus);
		goto out;
	}

	/*
	 * Open an fsp on this directory for the dptr.
	 */
	ntstatus = SMB_VFS_CREATE_FILE(
			conn, /* conn */
			req, /* req */
			smb_dname, /* dname */
			FILE_LIST_DIRECTORY, /* access_mask */
			FILE_SHARE_READ|
			FILE_SHARE_WRITE, /* share_access */
			FILE_OPEN, /* create_disposition*/
			FILE_DIRECTORY_FILE, /* create_options */
			FILE_ATTRIBUTE_DIRECTORY,/* file_attributes */
			NO_OPLOCK, /* oplock_request */
			NULL, /* lease */
			0, /* allocation_size */
			0, /* private_flags */
			NULL, /* sd */
			NULL, /* ea_list */
			&fsp, /* result */
			NULL, /* pinfo */
			NULL, /* in_context */
			NULL);/* out_context */

	if (!NT_STATUS_IS_OK(ntstatus)) {
		DBG_ERR("failed to open directory %s\n",
			smb_fname_str_dbg(smb_dname));
		reply_nterror(req, ntstatus);
		goto out;
	}

	/* Save the wildcard match and attribs we are using on this directory -
		needed as lanman2 assumes these are being saved between calls */

	ntstatus = dptr_create(conn,
				req,
				fsp, /* fsp */
				False,
				True,
				req->smbpid,
				mask,
				dirtype,
				&fsp->dptr);

	if (!NT_STATUS_IS_OK(ntstatus)) {
		/*
		 * Use NULL here for the first parameter (req)
		 * as this is not a client visible handle so
		 * can'tbe part of an SMB1 chain.
		 */
		close_file(NULL, fsp, NORMAL_CLOSE);
		fsp = NULL;
		reply_nterror(req, ntstatus);
		goto out;
	}

	if (backup_priv) {
		/* Remember this in case we have
		   to do a findnext. */
		dptr_set_priv(fsp->dptr);
	}

	dptr_num = dptr_dnum(fsp->dptr);
	DEBUG(4,("dptr_num is %d, wcard = %s, attr = %d\n", dptr_num, mask, dirtype));

	/* We don't need to check for VOL here as this is returned by
		a different TRANS2 call. */

	DEBUG(8,("dirpath=<%s> dontdescend=<%s>\n",
		 directory,lp_dont_descend(talloc_tos(), lp_sub, SNUM(conn))));
	if (in_list(directory,
		    lp_dont_descend(talloc_tos(), lp_sub, SNUM(conn)),
			conn->case_sensitive)) {
		dont_descend = True;
	}

	p = pdata;
	space_remaining = max_data_bytes;
	out_of_space = False;

	for (i=0;(i<maxentries) && !finished && !out_of_space;i++) {
		bool got_exact_match = False;

		/* this is a heuristic to avoid seeking the dirptr except when
			absolutely necessary. It allows for a filename of about 40 chars */
		if (space_remaining < DIRLEN_GUESS && numentries > 0) {
			out_of_space = True;
			finished = False;
		} else {
			ntstatus = get_lanman2_dir_entry(talloc_tos(),
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

		/* Ensure space_remaining never goes -ve. */
		if (PTR_DIFF(p,pdata) > max_data_bytes) {
			space_remaining = 0;
			out_of_space = true;
		} else {
			space_remaining = max_data_bytes - PTR_DIFF(p,pdata);
		}
	}

	/* Check if we can close the dirptr */
	if(close_after_first || (finished && close_if_end)) {
		DEBUG(5,("call_trans2findfirst - (2) closing dptr_num %d\n", dptr_num));
		dptr_num = -1;
		close_file(NULL, fsp, NORMAL_CLOSE);
		fsp = NULL;
	}

	/*
	 * If there are no matching entries we must return ERRDOS/ERRbadfile -
	 * from observation of NT. NB. This changes to ERRDOS,ERRnofiles if
	 * the protocol level is less than NT1. Tested with smbclient. JRA.
	 * This should fix the OS/2 client bug #2335.
	 */

	if(numentries == 0) {
		dptr_num = -1;
		/*
		 * We may have already closed the file in the
		 * close_after_first or finished case above.
		 */
		if (fsp != NULL) {
			close_file(NULL, fsp, NORMAL_CLOSE);
			fsp = NULL;
		}
		if (get_Protocol() < PROTOCOL_NT1) {
			reply_force_doserror(req, ERRDOS, ERRnofiles);
			goto out;
		} else {
			reply_botherror(req, NT_STATUS_NO_SUCH_FILE,
					ERRDOS, ERRbadfile);
			goto out;
		}
	}

	/* At this point pdata points to numentries directory entries. */

	/* Set up the return parameter block */
	SSVAL(params,0,dptr_num);
	SSVAL(params,2,numentries);
	SSVAL(params,4,finished);
	SSVAL(params,6,0); /* Never an EA error */
	SSVAL(params,8,last_entry_off);

	send_trans2_replies(conn, req, NT_STATUS_OK, params, 10, pdata, PTR_DIFF(p,pdata),
			    max_data_bytes);

	if ((! *directory) && dptr_path(sconn, dptr_num)) {
		directory = talloc_strdup(talloc_tos(),dptr_path(sconn, dptr_num));
		if (!directory) {
			reply_nterror(req, NT_STATUS_NO_MEMORY);
		}
	}

	DEBUG( 4, ( "%s mask=%s directory=%s dirtype=%d numentries=%d\n",
		smb_fn_name(req->cmd),
		mask, directory, dirtype, numentries ) );

	/*
	 * Force a name mangle here to ensure that the
	 * mask as an 8.3 name is top of the mangled cache.
	 * The reasons for this are subtle. Don't remove
	 * this code unless you know what you are doing
	 * (see PR#13758). JRA.
	 */

	if(!mangle_is_8_3_wildcards( mask, False, conn->params)) {
		char mangled_name[13];
		name_to_8_3(mask, mangled_name, True, conn->params);
	}
 out:

	if (as_root) {
		unbecome_root();
	}

	TALLOC_FREE(smb_dname);
	return;
}