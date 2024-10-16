static int vfswrap_ftruncate(vfs_handle_struct *handle, files_struct *fsp, off_t len)
{
	int result = -1;
	SMB_STRUCT_STAT *pst;
	NTSTATUS status;
	char c = 0;

	START_PROFILE(syscall_ftruncate);

	if (lp_strict_allocate(SNUM(fsp->conn)) && !fsp->is_sparse) {
		result = strict_allocate_ftruncate(handle, fsp, len);
		END_PROFILE(syscall_ftruncate);
		return result;
	}

	/* we used to just check HAVE_FTRUNCATE_EXTEND and only use
	   ftruncate if the system supports it. Then I discovered that
	   you can have some filesystems that support ftruncate
	   expansion and some that don't! On Linux fat can't do
	   ftruncate extend but ext2 can. */

	result = ftruncate(fsp->fh->fd, len);
	if (result == 0)
		goto done;

	/* According to W. R. Stevens advanced UNIX prog. Pure 4.3 BSD cannot
	   extend a file with ftruncate. Provide alternate implementation
	   for this */

	/* Do an fstat to see if the file is longer than the requested
	   size in which case the ftruncate above should have
	   succeeded or shorter, in which case seek to len - 1 and
	   write 1 byte of zero */
	status = vfs_stat_fsp(fsp);
	if (!NT_STATUS_IS_OK(status)) {
		goto done;
	}
	pst = &fsp->fsp_name->st;

#ifdef S_ISFIFO
	if (S_ISFIFO(pst->st_ex_mode)) {
		result = 0;
		goto done;
	}
#endif

	if (pst->st_ex_size == len) {
		result = 0;
		goto done;
	}

	if (pst->st_ex_size > len) {
		/* the ftruncate should have worked */
		goto done;
	}

	if (SMB_VFS_PWRITE(fsp, &c, 1, len-1)!=1) {
		goto done;
	}

	result = 0;

  done:

	END_PROFILE(syscall_ftruncate);
	return result;
}