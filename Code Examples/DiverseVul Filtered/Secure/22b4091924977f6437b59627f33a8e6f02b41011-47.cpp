static void call_trans2setfsinfo(connection_struct *conn,
				 struct smb_request *req,
				 char **pparams, int total_params,
				 char **ppdata, int total_data,
				 unsigned int max_data_bytes)
{
	const struct loadparm_substitution *lp_sub =
		loadparm_s3_global_substitution();
	struct smbXsrv_connection *xconn = req->xconn;
	char *pdata = *ppdata;
	char *params = *pparams;
	uint16_t info_level;

	DEBUG(10,("call_trans2setfsinfo: for service [%s]\n",
		  lp_servicename(talloc_tos(), lp_sub, SNUM(conn))));

	/*  */
	if (total_params < 4) {
		DEBUG(0,("call_trans2setfsinfo: requires total_params(%d) >= 4 bytes!\n",
			total_params));
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	info_level = SVAL(params,2);

	if (IS_IPC(conn)) {
		if (info_level != SMB_REQUEST_TRANSPORT_ENCRYPTION &&
				info_level != SMB_SET_CIFS_UNIX_INFO) {
			DEBUG(0,("call_trans2setfsinfo: not an allowed "
				"info level (0x%x) on IPC$.\n",
				(unsigned int)info_level));
			reply_nterror(req, NT_STATUS_ACCESS_DENIED);
			return;
		}
	}

	if (ENCRYPTION_REQUIRED(conn) && !req->encrypted) {
		if (info_level != SMB_REQUEST_TRANSPORT_ENCRYPTION) {
			DEBUG(0,("call_trans2setfsinfo: encryption required "
				"and info level 0x%x sent.\n",
				(unsigned int)info_level));
			reply_nterror(req, NT_STATUS_ACCESS_DENIED);
			return;
		}
	}

	switch(info_level) {
		case SMB_SET_CIFS_UNIX_INFO:
			if (!lp_unix_extensions()) {
				DEBUG(2,("call_trans2setfsinfo: "
					"SMB_SET_CIFS_UNIX_INFO is invalid with "
					"unix extensions off\n"));
				reply_nterror(req,
					      NT_STATUS_INVALID_LEVEL);
				return;
			}

			/* There should be 12 bytes of capabilities set. */
			if (total_data < 12) {
				reply_nterror(
					req,
					NT_STATUS_INVALID_PARAMETER);
				return;
			}
			xconn->smb1.unix_info.client_major = SVAL(pdata,0);
			xconn->smb1.unix_info.client_minor = SVAL(pdata,2);
			xconn->smb1.unix_info.client_cap_low = IVAL(pdata,4);
			xconn->smb1.unix_info.client_cap_high = IVAL(pdata,8);
			/* Just print these values for now. */
			DEBUG(10, ("call_trans2setfsinfo: set unix_info info. "
				   "major = %u, minor = %u cap_low = 0x%x, "
				   "cap_high = 0x%xn",
				   (unsigned int)xconn->
				   smb1.unix_info.client_major,
				   (unsigned int)xconn->
				   smb1.unix_info.client_minor,
				   (unsigned int)xconn->
				   smb1.unix_info.client_cap_low,
				   (unsigned int)xconn->
				   smb1.unix_info.client_cap_high));

			/* Here is where we must switch to posix pathname processing... */
			if (xconn->smb1.unix_info.client_cap_low & CIFS_UNIX_POSIX_PATHNAMES_CAP) {
				lp_set_posix_pathnames();
				mangle_change_to_posix();
			}

			if ((xconn->smb1.unix_info.client_cap_low & CIFS_UNIX_FCNTL_LOCKS_CAP) &&
			    !(xconn->smb1.unix_info.client_cap_low & CIFS_UNIX_POSIX_PATH_OPERATIONS_CAP)) {
				/* Client that knows how to do posix locks,
				 * but not posix open/mkdir operations. Set a
				 * default type for read/write checks. */

				lp_set_posix_default_cifsx_readwrite_locktype(POSIX_LOCK);

			}
			break;

		case SMB_REQUEST_TRANSPORT_ENCRYPTION:
			{
				NTSTATUS status;
				size_t param_len = 0;
				size_t data_len = total_data;

				if (!lp_unix_extensions()) {
					reply_nterror(
						req,
						NT_STATUS_INVALID_LEVEL);
					return;
				}

				if (lp_server_smb_encrypt(SNUM(conn)) ==
				    SMB_ENCRYPTION_OFF) {
					reply_nterror(
						req,
						NT_STATUS_NOT_SUPPORTED);
					return;
				}

				if (xconn->smb1.echo_handler.trusted_fde) {
					DEBUG( 2,("call_trans2setfsinfo: "
						"request transport encryption disabled"
						"with 'fork echo handler = yes'\n"));
					reply_nterror(
						req,
						NT_STATUS_NOT_SUPPORTED);
					return;
				}

				DEBUG( 4,("call_trans2setfsinfo: "
					"request transport encryption.\n"));

				status = srv_request_encryption_setup(conn,
								(unsigned char **)ppdata,
								&data_len,
								(unsigned char **)pparams,
								&param_len);

				if (!NT_STATUS_EQUAL(status, NT_STATUS_MORE_PROCESSING_REQUIRED) &&
						!NT_STATUS_IS_OK(status)) {
					reply_nterror(req, status);
					return;
				}

				send_trans2_replies(conn, req,
						NT_STATUS_OK,
						*pparams,
						param_len,
						*ppdata,
						data_len,
						max_data_bytes);

				if (NT_STATUS_IS_OK(status)) {
					/* Server-side transport
					 * encryption is now *on*. */
					status = srv_encryption_start(conn);
					if (!NT_STATUS_IS_OK(status)) {
						char *reason = talloc_asprintf(talloc_tos(),
									       "Failure in setting "
									       "up encrypted transport: %s",
									       nt_errstr(status));
						exit_server_cleanly(reason);
					}
				}
				return;
			}

		case SMB_FS_QUOTA_INFORMATION:
			{
				NTSTATUS status;
				DATA_BLOB qdata = {
						.data = (uint8_t *)pdata,
						.length = total_data
				};
				files_struct *fsp = NULL;
				fsp = file_fsp(req, SVAL(params,0));

				status = smb_set_fsquota(conn,
							req,
							fsp,
							&qdata);
				if (!NT_STATUS_IS_OK(status)) {
					reply_nterror(req, status);
					return;
				}
				break;
			}
		default:
			DEBUG(3,("call_trans2setfsinfo: unknown level (0x%X) not implemented yet.\n",
				info_level));
			reply_nterror(req, NT_STATUS_INVALID_LEVEL);
			return;
			break;
	}

	/* 
	 * sending this reply works fine, 
	 * but I'm not sure it's the same 
	 * like windows do...
	 * --metze
	 */
	reply_outbuf(req, 10, 0);
}