static void do_ntcreate_pipe_open(connection_struct *conn,
				  struct smb_request *req)
{
	char *fname = NULL;
	uint16_t pnum = FNUM_FIELD_INVALID;
	char *p = NULL;
	uint32 flags = IVAL(req->vwv+3, 1);
	TALLOC_CTX *ctx = talloc_tos();

	srvstr_pull_req_talloc(ctx, req, &fname, req->buf, STR_TERMINATE);

	if (!fname) {
		reply_botherror(req, NT_STATUS_OBJECT_NAME_NOT_FOUND,
				ERRDOS, ERRbadpipe);
		return;
	}
	nt_open_pipe(fname, conn, req, &pnum);

	if (req->outbuf) {
		/* error reply */
		return;
	}

	/*
	 * Deal with pipe return.
	 */

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
	p++;
	SSVAL(p,0,pnum);
	p += 2;
	SIVAL(p,0,FILE_WAS_OPENED);
	p += 4;
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

	DEBUG(5,("do_ntcreate_pipe_open: open pipe = %s\n", fname));
}