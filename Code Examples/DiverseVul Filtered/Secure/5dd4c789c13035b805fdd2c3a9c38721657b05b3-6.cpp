static struct files_struct *smbd_notifyd_reregister(struct files_struct *fsp,
						    void *private_data)
{
	DBG_DEBUG("reregister %s\n", fsp->fsp_name->base_name);

	if ((fsp->conn->sconn->notify_ctx != NULL) &&
	    (fsp->notify != NULL) &&
	    ((fsp->notify->filter != 0) ||
	     (fsp->notify->subdir_filter != 0))) {
		size_t len = fsp_fullbasepath(fsp, NULL, 0);
		char fullpath[len+1];

		NTSTATUS status;

		fsp_fullbasepath(fsp, fullpath, sizeof(fullpath));
		if (len > 1 && fullpath[len-1] == '.' &&
		    fullpath[len-2] == '/') {
			fullpath[len-2] = '\0';
		}

		status = notify_add(fsp->conn->sconn->notify_ctx,
				    fullpath, fsp->notify->filter,
				    fsp->notify->subdir_filter, fsp);
		if (!NT_STATUS_IS_OK(status)) {
			DBG_DEBUG("notify_add failed: %s\n",
				  nt_errstr(status));
		}
	}
	return NULL;
}