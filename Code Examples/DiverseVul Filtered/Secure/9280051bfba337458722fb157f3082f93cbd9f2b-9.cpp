static struct pending_auth_data *get_pending_auth_data(
		struct smbd_server_connection *sconn,
		uint16_t smbpid)
{
	struct pending_auth_data *pad;
/*
 * NOTE: using the smbpid here is completely wrong...
 *       see [MS-SMB]
 *       3.3.5.3 Receiving an SMB_COM_SESSION_SETUP_ANDX Request
 */
	for (pad = sconn->smb1.pd_list; pad; pad = pad->next) {
		if (pad->smbpid == smbpid) {
			break;
		}
	}
	return pad;
}