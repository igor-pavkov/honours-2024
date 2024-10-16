static int vfswrap_fchmod_acl(vfs_handle_struct *handle, files_struct *fsp, mode_t mode)
{
#ifdef HAVE_NO_ACL
	errno = ENOSYS;
	return -1;
#else
	int result;

	START_PROFILE(fchmod_acl);
	result = fchmod_acl(fsp, mode);
	END_PROFILE(fchmod_acl);
	return result;
#endif
}