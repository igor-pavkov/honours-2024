static int vfswrap_get_shadow_copy_data(struct vfs_handle_struct *handle,
					struct files_struct *fsp,
					struct shadow_copy_data *shadow_copy_data,
					bool labels)
{
	errno = ENOSYS;
	return -1;  /* Not implemented. */
}