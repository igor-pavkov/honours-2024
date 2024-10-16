void reply_findclose(struct smb_request *req)
{
	int dptr_num;
	struct smbd_server_connection *sconn = req->sconn;
	files_struct *fsp = NULL;

	START_PROFILE(SMBfindclose);

	if (req->wct < 1) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		END_PROFILE(SMBfindclose);
		return;
	}

	dptr_num = SVALS(req->vwv+0, 0);

	DEBUG(3,("reply_findclose, dptr_num = %d\n", dptr_num));

	/*
	 * OS/2 seems to use -1 to indicate "close all directories"
	 * This has to mean on this specific connection struct.
	 */
	if (dptr_num == -1) {
		dptr_closecnum(req->conn);
	} else {
		fsp = dptr_fetch_lanman2_fsp(sconn, dptr_num);
		dptr_num = -1;
		if (fsp != NULL) {
			close_file(NULL, fsp, NORMAL_CLOSE);
			fsp = NULL;
		}
	}

	reply_outbuf(req, 0, 0);

	DEBUG(3,("SMBfindclose dptr_num = %d\n", dptr_num));

	END_PROFILE(SMBfindclose);
	return;
}