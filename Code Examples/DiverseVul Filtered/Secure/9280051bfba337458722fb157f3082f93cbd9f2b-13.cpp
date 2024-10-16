void reply_sesssetup_and_X(struct smb_request *req)
{
	int sess_vuid;
	int smb_bufsize;
	DATA_BLOB lm_resp;
	DATA_BLOB nt_resp;
	DATA_BLOB plaintext_password;
	char *tmp;
	const char *user;
	fstring sub_user; /* Sainitised username for substituion */
	const char *domain;
	const char *native_os;
	const char *native_lanman;
	const char *primary_domain;
	auth_usersupplied_info *user_info = NULL;
	auth_serversupplied_info *server_info = NULL;
	uint16 smb_flag2 = req->flags2;

	NTSTATUS nt_status;
	struct smbd_server_connection *sconn = smbd_server_conn;

	bool doencrypt = sconn->smb1.negprot.encrypted_passwords;

	START_PROFILE(SMBsesssetupX);

	ZERO_STRUCT(lm_resp);
	ZERO_STRUCT(nt_resp);
	ZERO_STRUCT(plaintext_password);

	DEBUG(3,("wct=%d flg2=0x%x\n", req->wct, req->flags2));

	/* a SPNEGO session setup has 12 command words, whereas a normal
	   NT1 session setup has 13. See the cifs spec. */
	if (req->wct == 12 &&
	    (req->flags2 & FLAGS2_EXTENDED_SECURITY)) {

		if (!sconn->smb1.negprot.spnego) {
			DEBUG(0,("reply_sesssetup_and_X:  Rejecting attempt "
				 "at SPNEGO session setup when it was not "
				 "negotiated.\n"));
			reply_nterror(req, nt_status_squash(
					      NT_STATUS_LOGON_FAILURE));
			END_PROFILE(SMBsesssetupX);
			return;
		}

		if (SVAL(req->vwv+4, 0) == 0) {
			setup_new_vc_session();
		}

		reply_sesssetup_and_X_spnego(req);
		END_PROFILE(SMBsesssetupX);
		return;
	}

	smb_bufsize = SVAL(req->vwv+2, 0);

	if (get_Protocol() < PROTOCOL_NT1) {
		uint16 passlen1 = SVAL(req->vwv+7, 0);

		/* Never do NT status codes with protocols before NT1 as we
		 * don't get client caps. */
		remove_from_common_flags2(FLAGS2_32_BIT_ERROR_CODES);

		if ((passlen1 > MAX_PASS_LEN) || (passlen1 > req->buflen)) {
			reply_nterror(req, nt_status_squash(
					      NT_STATUS_INVALID_PARAMETER));
			END_PROFILE(SMBsesssetupX);
			return;
		}

		if (doencrypt) {
			lm_resp = data_blob(req->buf, passlen1);
		} else {
			plaintext_password = data_blob(req->buf, passlen1+1);
			/* Ensure null termination */
			plaintext_password.data[passlen1] = 0;
		}

		srvstr_pull_req_talloc(talloc_tos(), req, &tmp,
				       req->buf + passlen1, STR_TERMINATE);
		user = tmp ? tmp : "";

		domain = "";

	} else {
		uint16 passlen1 = SVAL(req->vwv+7, 0);
		uint16 passlen2 = SVAL(req->vwv+8, 0);
		enum remote_arch_types ra_type = get_remote_arch();
		const uint8_t *p = req->buf;
		const uint8_t *save_p = req->buf;
		uint16 byte_count;


		if(global_client_caps == 0) {
			global_client_caps = IVAL(req->vwv+11, 0);

			if (!(global_client_caps & CAP_STATUS32)) {
				remove_from_common_flags2(
						FLAGS2_32_BIT_ERROR_CODES);
			}

			/* client_caps is used as final determination if
			 * client is NT or Win95. This is needed to return
			 * the correct error codes in some circumstances.
			*/

			if(ra_type == RA_WINNT || ra_type == RA_WIN2K ||
					ra_type == RA_WIN95) {
				if(!(global_client_caps & (CAP_NT_SMBS|
							CAP_STATUS32))) {
					set_remote_arch( RA_WIN95);
				}
			}
		}

		if (!doencrypt) {
			/* both Win95 and WinNT stuff up the password
			 * lengths for non-encrypting systems. Uggh.

			   if passlen1==24 its a win95 system, and its setting
			   the password length incorrectly. Luckily it still
			   works with the default code because Win95 will null
			   terminate the password anyway

			   if passlen1>0 and passlen2>0 then maybe its a NT box
			   and its setting passlen2 to some random value which
			   really stuffs things up. we need to fix that one.  */

			if (passlen1 > 0 && passlen2 > 0 && passlen2 != 24 &&
					passlen2 != 1) {
				passlen2 = 0;
			}
		}

		/* check for nasty tricks */
		if (passlen1 > MAX_PASS_LEN
		    || passlen1 > smbreq_bufrem(req, p)) {
			reply_nterror(req, nt_status_squash(
					      NT_STATUS_INVALID_PARAMETER));
			END_PROFILE(SMBsesssetupX);
			return;
		}

		if (passlen2 > MAX_PASS_LEN
		    || passlen2 > smbreq_bufrem(req, p+passlen1)) {
			reply_nterror(req, nt_status_squash(
					      NT_STATUS_INVALID_PARAMETER));
			END_PROFILE(SMBsesssetupX);
			return;
		}

		/* Save the lanman2 password and the NT md4 password. */

		if ((doencrypt) && (passlen1 != 0) && (passlen1 != 24)) {
			doencrypt = False;
		}

		if (doencrypt) {
			lm_resp = data_blob(p, passlen1);
			nt_resp = data_blob(p+passlen1, passlen2);
		} else if (lp_security() != SEC_SHARE) {
			/*
			 * In share level we should ignore any passwords, so
 			 * only read them if we're not.
 			 */
			char *pass = NULL;
			bool unic= smb_flag2 & FLAGS2_UNICODE_STRINGS;

			if (unic && (passlen2 == 0) && passlen1) {
				/* Only a ascii plaintext password was sent. */
				(void)srvstr_pull_talloc(talloc_tos(),
							req->inbuf,
							req->flags2,
							&pass,
							req->buf,
							passlen1,
							STR_TERMINATE|STR_ASCII);
			} else {
				(void)srvstr_pull_talloc(talloc_tos(),
							req->inbuf,
							req->flags2,
							&pass,
							req->buf,
							unic ? passlen2 : passlen1,
							STR_TERMINATE);
			}
			if (!pass) {
				reply_nterror(req, nt_status_squash(
					      NT_STATUS_INVALID_PARAMETER));
				END_PROFILE(SMBsesssetupX);
				return;
			}
			plaintext_password = data_blob(pass, strlen(pass)+1);
		}

		p += passlen1 + passlen2;

		p += srvstr_pull_req_talloc(talloc_tos(), req, &tmp, p,
					    STR_TERMINATE);
		user = tmp ? tmp : "";

		p += srvstr_pull_req_talloc(talloc_tos(), req, &tmp, p,
					    STR_TERMINATE);
		domain = tmp ? tmp : "";

		p += srvstr_pull_req_talloc(talloc_tos(), req, &tmp, p,
					    STR_TERMINATE);
		native_os = tmp ? tmp : "";

		p += srvstr_pull_req_talloc(talloc_tos(), req, &tmp, p,
					    STR_TERMINATE);
		native_lanman = tmp ? tmp : "";

		/* not documented or decoded by Ethereal but there is one more
		 * string in the extra bytes which is the same as the
		 * PrimaryDomain when using extended security.  Windows NT 4
		 * and 2003 use this string to store the native lanman string.
		 * Windows 9x does not include a string here at all so we have
		 * to check if we have any extra bytes left */

		byte_count = SVAL(req->vwv+13, 0);
		if ( PTR_DIFF(p, save_p) < byte_count) {
			p += srvstr_pull_req_talloc(talloc_tos(), req, &tmp, p,
						    STR_TERMINATE);
			primary_domain = tmp ? tmp : "";
		} else {
			primary_domain = talloc_strdup(talloc_tos(), "null");
		}

		DEBUG(3,("Domain=[%s]  NativeOS=[%s] NativeLanMan=[%s] "
			"PrimaryDomain=[%s]\n",
			domain, native_os, native_lanman, primary_domain));

		if ( ra_type == RA_WIN2K ) {
			if ( strlen(native_lanman) == 0 )
				ra_lanman_string( primary_domain );
			else
				ra_lanman_string( native_lanman );
		}

	}

	if (SVAL(req->vwv+4, 0) == 0) {
		setup_new_vc_session();
	}

	DEBUG(3,("sesssetupX:name=[%s]\\[%s]@[%s]\n",
				domain, user, get_remote_machine_name()));

	if (*user) {
		if (sconn->smb1.negprot.spnego) {

			/* This has to be here, because this is a perfectly
			 * valid behaviour for guest logons :-( */

			DEBUG(0,("reply_sesssetup_and_X:  Rejecting attempt "
				"at 'normal' session setup after "
				"negotiating spnego.\n"));
			reply_nterror(req, nt_status_squash(
					      NT_STATUS_LOGON_FAILURE));
			END_PROFILE(SMBsesssetupX);
			return;
		}
		fstrcpy(sub_user, user);
	} else {
		fstrcpy(sub_user, lp_guestaccount());
	}

	sub_set_smb_name(sub_user);

	reload_services(True);

	if (lp_security() == SEC_SHARE) {
		/* In share level we should ignore any passwords */

		data_blob_free(&lm_resp);
		data_blob_free(&nt_resp);
		data_blob_clear_free(&plaintext_password);

		map_username(sconn, sub_user);
		add_session_user(sconn, sub_user);
		add_session_workgroup(sconn, domain);
		/* Then force it to null for the benfit of the code below */
		user = "";
	}

	if (!*user) {

		nt_status = check_guest_password(&server_info);

	} else if (doencrypt) {
		struct auth_context *negprot_auth_context = NULL;
		negprot_auth_context = sconn->smb1.negprot.auth_context;
		if (!negprot_auth_context) {
			DEBUG(0, ("reply_sesssetup_and_X:  Attempted encrypted "
				"session setup without negprot denied!\n"));
			reply_nterror(req, nt_status_squash(
					      NT_STATUS_LOGON_FAILURE));
			END_PROFILE(SMBsesssetupX);
			return;
		}
		nt_status = make_user_info_for_reply_enc(&user_info, user,
						domain,
						lm_resp, nt_resp);
		if (NT_STATUS_IS_OK(nt_status)) {
			nt_status = negprot_auth_context->check_ntlm_password(
					negprot_auth_context,
					user_info,
					&server_info);
		}
	} else {
		struct auth_context *plaintext_auth_context = NULL;

		nt_status = make_auth_context_subsystem(
				&plaintext_auth_context);

		if (NT_STATUS_IS_OK(nt_status)) {
			uint8_t chal[8];

			plaintext_auth_context->get_ntlm_challenge(
					plaintext_auth_context, chal);

			if (!make_user_info_for_reply(&user_info,
						      user, domain, chal,
						      plaintext_password)) {
				nt_status = NT_STATUS_NO_MEMORY;
			}

			if (NT_STATUS_IS_OK(nt_status)) {
				nt_status = plaintext_auth_context->check_ntlm_password(
						plaintext_auth_context,
						user_info,
						&server_info);

				(plaintext_auth_context->free)(
						&plaintext_auth_context);
			}
		}
	}

	free_user_info(&user_info);

	if (!NT_STATUS_IS_OK(nt_status)) {
		nt_status = do_map_to_guest(nt_status, &server_info,
				user, domain);
	}

	if (!NT_STATUS_IS_OK(nt_status)) {
		data_blob_free(&nt_resp);
		data_blob_free(&lm_resp);
		data_blob_clear_free(&plaintext_password);
		reply_nterror(req, nt_status_squash(nt_status));
		END_PROFILE(SMBsesssetupX);
		return;
	}

	/* Ensure we can't possible take a code path leading to a
	 * null defref. */
	if (!server_info) {
		reply_nterror(req, nt_status_squash(NT_STATUS_LOGON_FAILURE));
		END_PROFILE(SMBsesssetupX);
		return;
	}

	if (!server_info->ptok) {
		nt_status = create_local_token(server_info);

		if (!NT_STATUS_IS_OK(nt_status)) {
			DEBUG(10, ("create_local_token failed: %s\n",
				   nt_errstr(nt_status)));
			data_blob_free(&nt_resp);
			data_blob_free(&lm_resp);
			data_blob_clear_free(&plaintext_password);
			reply_nterror(req, nt_status_squash(nt_status));
			END_PROFILE(SMBsesssetupX);
			return;
		}
	}

	data_blob_clear_free(&plaintext_password);

	/* it's ok - setup a reply */
	reply_outbuf(req, 3, 0);
	if (get_Protocol() >= PROTOCOL_NT1) {
		push_signature(&req->outbuf);
		/* perhaps grab OS version here?? */
	}

	if (server_info->guest) {
		SSVAL(req->outbuf,smb_vwv2,1);
	}

	/* register the name and uid as being validated, so further connections
	   to a uid can get through without a password, on the same VC */

	if (lp_security() == SEC_SHARE) {
		sess_vuid = UID_FIELD_INVALID;
		TALLOC_FREE(server_info);
	} else {
		/* Ignore the initial vuid. */
		sess_vuid = register_initial_vuid(sconn);
		if (sess_vuid == UID_FIELD_INVALID) {
			data_blob_free(&nt_resp);
			data_blob_free(&lm_resp);
			reply_nterror(req, nt_status_squash(
					      NT_STATUS_LOGON_FAILURE));
			END_PROFILE(SMBsesssetupX);
			return;
		}
		/* register_existing_vuid keeps the server info */
		sess_vuid = register_existing_vuid(sconn, sess_vuid,
					server_info,
					nt_resp.data ? nt_resp : lm_resp,
					sub_user);
		if (sess_vuid == UID_FIELD_INVALID) {
			data_blob_free(&nt_resp);
			data_blob_free(&lm_resp);
			reply_nterror(req, nt_status_squash(
					      NT_STATUS_LOGON_FAILURE));
			END_PROFILE(SMBsesssetupX);
			return;
		}

		/* current_user_info is changed on new vuid */
		reload_services( True );
	}

	data_blob_free(&nt_resp);
	data_blob_free(&lm_resp);

	SSVAL(req->outbuf,smb_uid,sess_vuid);
	SSVAL(req->inbuf,smb_uid,sess_vuid);
	req->vuid = sess_vuid;

	if (!sconn->smb1.sessions.done_sesssetup) {
		sconn->smb1.sessions.max_send =
			MIN(sconn->smb1.sessions.max_send,smb_bufsize);
	}
	sconn->smb1.sessions.done_sesssetup = true;

	END_PROFILE(SMBsesssetupX);
	chain_reply(req);
	return;
}