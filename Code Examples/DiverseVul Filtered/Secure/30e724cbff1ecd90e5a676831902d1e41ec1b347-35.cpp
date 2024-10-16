static int vfswrap_set_offline(struct vfs_handle_struct *handle,
			       const struct smb_filename *fname)
{
	/* We don't know how to set offline bit by default, needs to be overriden in the vfs modules */
#if defined(ENOTSUP)
	errno = ENOTSUP;
#endif
	return -1;
}