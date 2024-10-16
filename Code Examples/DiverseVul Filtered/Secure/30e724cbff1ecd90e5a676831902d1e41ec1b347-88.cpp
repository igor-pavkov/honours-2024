static bool vfswrap_brl_cancel_windows(struct vfs_handle_struct *handle,
				       struct byte_range_lock *br_lck,
				       struct lock_struct *plock,
				       struct blocking_lock_record *blr)
{
	SMB_ASSERT(plock->lock_flav == WINDOWS_LOCK);

	/* Note: blr is not used in the default implementation. */
	return brl_lock_cancel_default(br_lck, plock);
}