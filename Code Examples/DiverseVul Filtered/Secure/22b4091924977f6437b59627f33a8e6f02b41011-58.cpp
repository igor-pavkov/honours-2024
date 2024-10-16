NTSTATUS smb_set_file_time(connection_struct *conn,
			   files_struct *fsp,
			   struct smb_filename *smb_fname,
			   struct smb_file_time *ft,
			   bool setting_write_time)
{
	struct files_struct *set_fsp = NULL;
	struct timeval_buf tbuf[4];
	uint32_t action =
		FILE_NOTIFY_CHANGE_LAST_ACCESS
		|FILE_NOTIFY_CHANGE_LAST_WRITE
		|FILE_NOTIFY_CHANGE_CREATION;
	int ret;

	if (!VALID_STAT(smb_fname->st)) {
		return NT_STATUS_OBJECT_NAME_NOT_FOUND;
	}

	if (fsp == NULL) {
		/* A symlink */
		return NT_STATUS_OK;
	}

	set_fsp = fsp->base_fsp == NULL ? fsp : fsp->base_fsp;

	/* get some defaults (no modifications) if any info is zero or -1. */
	if (is_omit_timespec(&ft->create_time)) {
		action &= ~FILE_NOTIFY_CHANGE_CREATION;
	}

	if (is_omit_timespec(&ft->atime)) {
		action &= ~FILE_NOTIFY_CHANGE_LAST_ACCESS;
	}

	if (is_omit_timespec(&ft->mtime)) {
		action &= ~FILE_NOTIFY_CHANGE_LAST_WRITE;
	}

	if (!setting_write_time) {
		/* ft->mtime comes from change time, not write time. */
		action &= ~FILE_NOTIFY_CHANGE_LAST_WRITE;
	}

	/* Ensure the resolution is the correct for
	 * what we can store on this filesystem. */

	round_timespec(conn->ts_res, &ft->create_time);
	round_timespec(conn->ts_res, &ft->ctime);
	round_timespec(conn->ts_res, &ft->atime);
	round_timespec(conn->ts_res, &ft->mtime);

	DBG_DEBUG("smb_set_filetime: actime: %s\n ",
		  timespec_string_buf(&ft->atime, true, &tbuf[0]));
	DBG_DEBUG("smb_set_filetime: modtime: %s\n ",
		  timespec_string_buf(&ft->mtime, true, &tbuf[1]));
	DBG_DEBUG("smb_set_filetime: ctime: %s\n ",
		  timespec_string_buf(&ft->ctime, true, &tbuf[2]));
	DBG_DEBUG("smb_set_file_time: createtime: %s\n ",
		  timespec_string_buf(&ft->create_time, true, &tbuf[3]));

	if (setting_write_time) {
		/*
		 * This was a Windows setfileinfo on an open file.
		 * NT does this a lot. We also need to 
		 * set the time here, as it can be read by 
		 * FindFirst/FindNext and with the patch for bug #2045
		 * in smbd/fileio.c it ensures that this timestamp is
		 * kept sticky even after a write. We save the request
		 * away and will set it on file close and after a write. JRA.
		 */

		DBG_DEBUG("setting pending modtime to %s\n",
			  timespec_string_buf(&ft->mtime, true, &tbuf[0]));

		if (set_fsp != NULL) {
			set_sticky_write_time_fsp(set_fsp, ft->mtime);
		} else {
			set_sticky_write_time_path(
				vfs_file_id_from_sbuf(conn, &smb_fname->st),
				ft->mtime);
		}
	}

	DEBUG(10,("smb_set_file_time: setting utimes to modified values.\n"));

	ret = file_ntimes(conn, set_fsp, ft);
	if (ret != 0) {
		return map_nt_error_from_unix(errno);
	}

	notify_fname(conn, NOTIFY_ACTION_MODIFIED, action,
		     smb_fname->base_name);
	return NT_STATUS_OK;
}