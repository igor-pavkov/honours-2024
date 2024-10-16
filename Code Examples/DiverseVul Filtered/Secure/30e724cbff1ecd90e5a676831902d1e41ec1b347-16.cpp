static const char *vfswrap_connectpath(struct vfs_handle_struct *handle,
				       const char *fname)
{
	return handle->conn->connectpath;
}