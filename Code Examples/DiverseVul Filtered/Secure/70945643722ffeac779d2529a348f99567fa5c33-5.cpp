int cifs_setup_session(unsigned int xid, struct cifsSesInfo *ses,
			struct nls_table *nls_info)
{
	int rc = 0;
	struct TCP_Server_Info *server = ses->server;

	ses->flags = 0;
	ses->capabilities = server->capabilities;
	if (linuxExtEnabled == 0)
		ses->capabilities &= (~CAP_UNIX);

	cFYI(1, "Security Mode: 0x%x Capabilities: 0x%x TimeAdjust: %d",
		 server->secMode, server->capabilities, server->timeAdj);

	rc = CIFS_SessSetup(xid, ses, nls_info);
	if (rc) {
		cERROR(1, "Send error in SessSetup = %d", rc);
	} else {
		mutex_lock(&ses->server->srv_mutex);
		if (!server->session_estab) {
			server->session_key.response = ses->auth_key.response;
			server->session_key.len = ses->auth_key.len;
			server->sequence_number = 0x2;
			server->session_estab = true;
			ses->auth_key.response = NULL;
		}
		mutex_unlock(&server->srv_mutex);

		cFYI(1, "CIFS Session Established successfully");
		spin_lock(&GlobalMid_Lock);
		ses->status = CifsGood;
		ses->need_reconnect = false;
		spin_unlock(&GlobalMid_Lock);
	}

	kfree(ses->auth_key.response);
	ses->auth_key.response = NULL;
	ses->auth_key.len = 0;
	kfree(ses->ntlmssp);
	ses->ntlmssp = NULL;

	return rc;
}