static NTSTATUS vfswrap_translate_name(struct vfs_handle_struct *handle,
				       const char *name,
				       enum vfs_translate_direction direction,
				       TALLOC_CTX *mem_ctx,
				       char **mapped_name)
{
	return NT_STATUS_NONE_MAPPED;
}