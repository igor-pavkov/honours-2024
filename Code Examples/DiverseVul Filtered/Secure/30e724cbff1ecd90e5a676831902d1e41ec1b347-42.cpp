static int vfswrap_lstat(vfs_handle_struct *handle,
			 struct smb_filename *smb_fname)
{
	int result = -1;

	START_PROFILE(syscall_lstat);

	if (smb_fname->stream_name) {
		errno = ENOENT;
		goto out;
	}

	result = sys_lstat(smb_fname->base_name, &smb_fname->st,
			   lp_fake_dir_create_times(SNUM(handle->conn)));
 out:
	END_PROFILE(syscall_lstat);
	return result;
}