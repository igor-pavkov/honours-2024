int cifs_negotiate_protocol(unsigned int xid, struct cifsSesInfo *ses)
{
	int rc = 0;
	struct TCP_Server_Info *server = ses->server;

	/* only send once per connect */
	if (server->maxBuf != 0)
		return 0;

	rc = CIFSSMBNegotiate(xid, ses);
	if (rc == -EAGAIN) {
		/* retry only once on 1st time connection */
		rc = CIFSSMBNegotiate(xid, ses);
		if (rc == -EAGAIN)
			rc = -EHOSTDOWN;
	}
	if (rc == 0) {
		spin_lock(&GlobalMid_Lock);
		if (server->tcpStatus != CifsExiting)
			server->tcpStatus = CifsGood;
		else
			rc = -EHOSTDOWN;
		spin_unlock(&GlobalMid_Lock);

	}

	return rc;
}