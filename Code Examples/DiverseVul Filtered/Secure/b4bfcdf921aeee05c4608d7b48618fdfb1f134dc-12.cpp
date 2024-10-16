static void nt_open_pipe(char *fname, connection_struct *conn,
			 struct smb_request *req, uint16_t *ppnum)
{
	files_struct *fsp;
	NTSTATUS status;

	DEBUG(4,("nt_open_pipe: Opening pipe %s.\n", fname));

	/* Strip \\ off the name if present. */
	while (fname[0] == '\\') {
		fname++;
	}

	status = open_np_file(req, fname, &fsp);
	if (!NT_STATUS_IS_OK(status)) {
		if (NT_STATUS_EQUAL(status, NT_STATUS_OBJECT_NAME_NOT_FOUND)) {
			reply_botherror(req, NT_STATUS_OBJECT_NAME_NOT_FOUND,
					ERRDOS, ERRbadpipe);
			return;
		}
		reply_nterror(req, status);
		return;
	}

	*ppnum = fsp->fnum;
	return;
}