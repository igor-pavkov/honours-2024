static void call_trans2mkdir(connection_struct *conn, struct smb_request *req,
			     char **pparams, int total_params,
			     char **ppdata, int total_data,
			     unsigned int max_data_bytes)
{
	struct files_struct *fsp = NULL;
	struct smb_filename *smb_dname = NULL;
	char *params = *pparams;
	char *pdata = *ppdata;
	char *directory = NULL;
	NTSTATUS status = NT_STATUS_OK;
	struct ea_list *ea_list = NULL;
	uint32_t ucf_flags = ucf_flags_from_smb_request(req);
	TALLOC_CTX *ctx = talloc_tos();

	if (!CAN_WRITE(conn)) {
		reply_nterror(req, NT_STATUS_ACCESS_DENIED);
		return;
	}

	if (total_params < 5) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	if (req->posix_pathnames) {
		srvstr_get_path_posix(ctx,
			params,
			req->flags2,
			&directory,
			&params[4],
			total_params - 4,
			STR_TERMINATE,
			&status);
	} else {
		srvstr_get_path(ctx,
			params,
			req->flags2,
			&directory,
			&params[4],
			total_params - 4,
			STR_TERMINATE,
			&status);
	}
	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		return;
	}

	DEBUG(3,("call_trans2mkdir : name = %s\n", directory));

	status = filename_convert(ctx,
				conn,
				directory,
				ucf_flags,
				0,
				&smb_dname);

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
	 * OS/2 workplace shell seems to send SET_EA requests of "null"
	 * length (4 bytes containing IVAL 4).
	 * They seem to have no effect. Bug #3212. JRA.
	 */

	if (total_data && (total_data != 4)) {
		/* Any data in this call is an EA list. */
		if (total_data < 10) {
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			goto out;
		}

		if (IVAL(pdata,0) > total_data) {
			DEBUG(10,("call_trans2mkdir: bad total data size (%u) > %u\n",
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
	}
	/* If total_data == 4 Windows doesn't care what values
	 * are placed in that field, it just ignores them.
	 * The System i QNTC IBM SMB client puts bad values here,
	 * so ignore them. */

	status = SMB_VFS_CREATE_FILE(
		conn,					/* conn */
		req,					/* req */
		smb_dname,				/* fname */
		MAXIMUM_ALLOWED_ACCESS,			/* access_mask */
		FILE_SHARE_NONE,			/* share_access */
		FILE_CREATE,				/* create_disposition*/
		FILE_DIRECTORY_FILE,			/* create_options */
		FILE_ATTRIBUTE_DIRECTORY,		/* file_attributes */
		0,					/* oplock_request */
		NULL,					/* lease */
		0,					/* allocation_size */
		0,					/* private_flags */
		NULL,					/* sd */
		NULL,					/* ea_list */
		&fsp,					/* result */
		NULL,					/* pinfo */
		NULL, NULL);				/* create context */
	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		goto out;
	}

	/* Try and set any given EA. */
	if (ea_list) {
		status = set_ea(conn, fsp, ea_list);
		if (!NT_STATUS_IS_OK(status)) {
			reply_nterror(req, status);
			goto out;
		}
	}

	/* Realloc the parameter and data sizes */
	*pparams = (char *)SMB_REALLOC(*pparams,2);
	if(*pparams == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		goto out;
	}
	params = *pparams;

	SSVAL(params,0,0);

	send_trans2_replies(conn, req, NT_STATUS_OK, params, 2, *ppdata, 0, max_data_bytes);

 out:
	if (fsp != NULL) {
		close_file(NULL, fsp, NORMAL_CLOSE);
		fsp = NULL;
	}
	TALLOC_FREE(smb_dname);
	return;
}