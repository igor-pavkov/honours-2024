static bool vfswrap_brl_unlock_windows(struct vfs_handle_struct *handle,
				       struct messaging_context *msg_ctx,
				       struct byte_range_lock *br_lck,
			               const struct lock_struct *plock)
{
	SMB_ASSERT(plock->lock_flav == WINDOWS_LOCK);

	return brl_unlock_windows_default(msg_ctx, br_lck, plock);
}