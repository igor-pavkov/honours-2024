static int vfswrap_fstat(vfs_handle_struct *handle, files_struct *fsp, SMB_STRUCT_STAT *sbuf)
{
	int result;

	START_PROFILE(syscall_fstat);
	result = sys_fstat(fsp->fh->fd,
			   sbuf, lp_fake_dir_create_times(SNUM(handle->conn)));
	END_PROFILE(syscall_fstat);
	return result;
}