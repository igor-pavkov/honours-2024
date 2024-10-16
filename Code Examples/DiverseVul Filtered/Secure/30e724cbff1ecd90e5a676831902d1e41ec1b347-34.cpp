static int vfswrap_unlink(vfs_handle_struct *handle,
			  const struct smb_filename *smb_fname)
{
	int result = -1;

	START_PROFILE(syscall_unlink);

	if (smb_fname->stream_name) {
		errno = ENOENT;
		goto out;
	}
	result = unlink(smb_fname->base_name);

 out:
	END_PROFILE(syscall_unlink);
	return result;
}