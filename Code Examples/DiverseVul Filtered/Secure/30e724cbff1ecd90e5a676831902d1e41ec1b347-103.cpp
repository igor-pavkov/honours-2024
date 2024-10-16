static NTSTATUS vfswrap_notify_watch(vfs_handle_struct *vfs_handle,
				     struct sys_notify_context *ctx,
				     const char *path,
				     uint32_t *filter,
				     uint32_t *subdir_filter,
				     void (*callback)(struct sys_notify_context *ctx, 
						      void *private_data,
						      struct notify_event *ev),
				     void *private_data, void *handle)
{
	/*
	 * So far inotify is the only supported default notify mechanism. If
	 * another platform like the the BSD's or a proprietary Unix comes
	 * along and wants another default, we can play the same trick we
	 * played with Posix ACLs.
	 *
	 * Until that is the case, hard-code inotify here.
	 */
#ifdef HAVE_INOTIFY
	if (lp_kernel_change_notify(vfs_handle->conn->params)) {
		return inotify_watch(ctx, path, filter, subdir_filter,
				     callback, private_data, handle);
	}
#endif
	/*
	 * Do nothing, leave everything to notify_internal.c
	 */
	return NT_STATUS_OK;
}