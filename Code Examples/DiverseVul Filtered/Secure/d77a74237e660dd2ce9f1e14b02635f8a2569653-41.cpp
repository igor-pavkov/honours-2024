void kernel_flock(int fd, uint32 share_mode, uint32 access_mask)
{
#if HAVE_KERNEL_SHARE_MODES
	int kernel_mode = 0;
	if (share_mode == FILE_SHARE_WRITE) {
		kernel_mode = LOCK_MAND|LOCK_WRITE;
	} else if (share_mode == FILE_SHARE_READ) {
		kernel_mode = LOCK_MAND|LOCK_READ;
	} else if (share_mode == FILE_SHARE_NONE) {
		kernel_mode = LOCK_MAND;
	}
	if (kernel_mode) {
		flock(fd, kernel_mode);
	}
#endif
	;
}