static void vfswrap_rewinddir(vfs_handle_struct *handle, DIR *dirp)
{
	START_PROFILE(syscall_rewinddir);
	rewinddir(dirp);
	END_PROFILE(syscall_rewinddir);
}