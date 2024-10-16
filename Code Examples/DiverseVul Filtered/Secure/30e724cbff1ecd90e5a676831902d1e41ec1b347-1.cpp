static int vfswrap_open(vfs_handle_struct *handle,
			struct smb_filename *smb_fname,
			files_struct *fsp, int flags, mode_t mode)
{
	int result = -1;

	START_PROFILE(syscall_open);

	if (smb_fname->stream_name) {
		errno = ENOENT;
		goto out;
	}

	result = open(smb_fname->base_name, flags, mode);
 out:
	END_PROFILE(syscall_open);
	return result;
}