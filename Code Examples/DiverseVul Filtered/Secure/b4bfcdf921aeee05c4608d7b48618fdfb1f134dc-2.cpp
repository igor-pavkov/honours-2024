static void do_nt_transact_create_pipe(connection_struct *conn,
				       struct smb_request *req,
				       uint16 **ppsetup, uint32 setup_count,
				       char **ppparams, uint32 parameter_count,
				       char **ppdata, uint32 data_count)
{
	char *fname = NULL;
	char *params = *ppparams;
	uint16_t pnum = FNUM_FIELD_INVALID;
	char *p = NULL;
	NTSTATUS status;
	size_t param_len;
	uint32 flags;
	TALLOC_CTX *ctx = talloc_tos();

	/*
	 * Ensure minimum number of parameters sent.
	 */

	if(parameter_count < 54) {
		DEBUG(0,("do_nt_transact_create_pipe - insufficient parameters (%u)\n", (unsigned int)parameter_count));
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	flags = IVAL(params,0);

	srvstr_get_path(ctx, params, req->flags2, &fname, params+53,
			parameter_count-53, STR_TERMINATE,
			&status);
	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		return;
	}

	nt_open_pipe(fname, conn, req, &pnum);

	if (req->outbuf) {
		/* Error return */
		return;
	}

	/* Realloc the size of parameters and data we will return */
	if (flags & EXTENDED_RESPONSE_REQUIRED) {
		/* Extended response is 32 more byyes. */
		param_len = 101;
	} else {
		param_len = 69;
	}
	params = nttrans_realloc(ppparams, param_len);
	if(params == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		return;
	}

	p = params;
	SCVAL(p,0,NO_OPLOCK_RETURN);

	p += 2;
	SSVAL(p,0,pnum);
	p += 2;
	SIVAL(p,0,FILE_WAS_OPENED);
	p += 8;

	p += 32;
	SIVAL(p,0,FILE_ATTRIBUTE_NORMAL); /* File Attributes. */
	p += 20;
	/* File type. */
	SSVAL(p,0,FILE_TYPE_MESSAGE_MODE_PIPE);
	/* Device state. */
	SSVAL(p,2, 0x5FF); /* ? */
	p += 4;

	if (flags & EXTENDED_RESPONSE_REQUIRED) {
		p += 25;
		SIVAL(p,0,FILE_GENERIC_ALL);
		/*
		 * For pipes W2K3 seems to return
 		 * 0x12019B next.
 		 * This is ((FILE_GENERIC_READ|FILE_GENERIC_WRITE) & ~FILE_APPEND_DATA)
 		 */
		SIVAL(p,4,(FILE_GENERIC_READ|FILE_GENERIC_WRITE)&~FILE_APPEND_DATA);
	}

	DEBUG(5,("do_nt_transact_create_pipe: open name = %s\n", fname));

	/* Send the required number of replies */
	send_nt_replies(conn, req, NT_STATUS_OK, params, param_len, *ppdata, 0);

	return;
}