static void delete_partial_auth(struct smbd_server_connection *sconn,
				struct pending_auth_data *pad)
{
	if (!pad) {
		return;
	}
	DLIST_REMOVE(sconn->smb1.pd_list, pad);
	data_blob_free(&pad->partial_data);
	SAFE_FREE(pad);
}