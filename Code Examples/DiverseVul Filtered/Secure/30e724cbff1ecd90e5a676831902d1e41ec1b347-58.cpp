static NTSTATUS vfswrap_brl_lock_windows(struct vfs_handle_struct *handle,
					 struct byte_range_lock *br_lck,
					 struct lock_struct *plock,
					 bool blocking_lock,
					 struct blocking_lock_record *blr)
{
	SMB_ASSERT(plock->lock_flav == WINDOWS_LOCK);

	/* Note: blr is not used in the default implementation. */
	return brl_lock_windows_default(br_lck, plock, blocking_lock);
}