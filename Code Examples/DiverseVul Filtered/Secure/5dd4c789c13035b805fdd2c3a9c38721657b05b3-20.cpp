static struct files_struct *smbd_notify_cancel_deleted_fn(
	struct files_struct *fsp, void *private_data)
{
	struct file_id *fid = talloc_get_type_abort(
		private_data, struct file_id);

	if (file_id_equal(&fsp->file_id, fid)) {
		remove_pending_change_notify_requests_by_fid(
			fsp, NT_STATUS_DELETE_PENDING);
	}
	return NULL;
}