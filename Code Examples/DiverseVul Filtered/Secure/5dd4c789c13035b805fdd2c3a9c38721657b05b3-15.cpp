void change_notify_reply(struct smb_request *req,
			 NTSTATUS error_code,
			 uint32_t max_param,
			 struct notify_change_buf *notify_buf,
			 void (*reply_fn)(struct smb_request *req,
					  NTSTATUS error_code,
					  uint8_t *buf, size_t len))
{
	DATA_BLOB blob = data_blob_null;

	if (!NT_STATUS_IS_OK(error_code)) {
		reply_fn(req, error_code, NULL, 0);
		return;
	}

	if (notify_buf == NULL) {
		reply_fn(req, NT_STATUS_OK, NULL, 0);
		return;
	}

	max_param = MIN(max_param, notify_buf->max_buffer_size);

	if (!notify_marshall_changes(notify_buf->num_changes, max_param,
					notify_buf->changes, &blob)) {
		/*
		 * We exceed what the client is willing to accept. Send
		 * nothing.
		 */
		data_blob_free(&blob);
	}

	reply_fn(req, NT_STATUS_OK, blob.data, blob.length);

	data_blob_free(&blob);

	TALLOC_FREE(notify_buf->changes);
	notify_buf->num_changes = 0;
}