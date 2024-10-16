void smbd_notify_cancel_deleted(struct messaging_context *msg,
				void *private_data, uint32_t msg_type,
				struct server_id server_id, DATA_BLOB *data)
{
	struct smbd_server_connection *sconn = talloc_get_type_abort(
		private_data, struct smbd_server_connection);
	struct file_id *fid;
	enum ndr_err_code ndr_err;

	fid = talloc(talloc_tos(), struct file_id);
	if (fid == NULL) {
		DEBUG(1, ("talloc failed\n"));
		return;
	}

	ndr_err = ndr_pull_struct_blob_all(
		data, fid, fid, (ndr_pull_flags_fn_t)ndr_pull_file_id);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		DEBUG(10, ("%s: ndr_pull_file_id failed: %s\n", __func__,
			   ndr_errstr(ndr_err)));
		goto done;
	}

	files_forall(sconn, smbd_notify_cancel_deleted_fn, fid);

done:
	TALLOC_FREE(fid);
}