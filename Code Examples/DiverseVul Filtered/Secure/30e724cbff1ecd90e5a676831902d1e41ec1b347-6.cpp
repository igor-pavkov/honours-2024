static int vfswrap_kernel_flock(vfs_handle_struct *handle, files_struct *fsp,
				uint32 share_mode, uint32 access_mask)
{
	START_PROFILE(syscall_kernel_flock);
	kernel_flock(fsp->fh->fd, share_mode, access_mask);
	END_PROFILE(syscall_kernel_flock);
	return 0;
}