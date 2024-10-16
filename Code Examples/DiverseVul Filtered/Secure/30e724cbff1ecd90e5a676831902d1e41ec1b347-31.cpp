static NTSTATUS vfswrap_audit_file(struct vfs_handle_struct *handle,
				   struct smb_filename *file,
				   struct security_acl *sacl,
				   uint32_t access_requested,
				   uint32_t access_denied)
{
	return NT_STATUS_OK; /* Nothing to do here ... */
}