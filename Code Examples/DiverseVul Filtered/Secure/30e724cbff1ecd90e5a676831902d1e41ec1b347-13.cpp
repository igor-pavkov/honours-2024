static int vfswrap_rename(vfs_handle_struct *handle,
			  const struct smb_filename *smb_fname_src,
			  const struct smb_filename *smb_fname_dst)
{
	int result = -1;

	START_PROFILE(syscall_rename);

	if (smb_fname_src->stream_name || smb_fname_dst->stream_name) {
		errno = ENOENT;
		goto out;
	}

	result = rename(smb_fname_src->base_name, smb_fname_dst->base_name);

 out:
	END_PROFILE(syscall_rename);
	return result;
}