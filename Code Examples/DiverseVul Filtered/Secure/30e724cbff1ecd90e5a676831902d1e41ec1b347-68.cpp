static bool vfswrap_strict_lock(struct vfs_handle_struct *handle,
				files_struct *fsp,
				struct lock_struct *plock)
{
	SMB_ASSERT(plock->lock_type == READ_LOCK ||
	    plock->lock_type == WRITE_LOCK);

	return strict_lock_default(fsp, plock);
}