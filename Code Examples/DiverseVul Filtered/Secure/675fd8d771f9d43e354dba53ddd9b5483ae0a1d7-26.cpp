static char *have_snapdir(struct vfs_handle_struct *handle,
			  const char *path)
{
	struct smb_filename smb_fname;
	int ret;
	struct shadow_copy2_config *config;

	SMB_VFS_HANDLE_GET_DATA(handle, config, struct shadow_copy2_config,
				return NULL);

	ZERO_STRUCT(smb_fname);
	smb_fname.base_name = talloc_asprintf(talloc_tos(), "%s/%s",
					      path, config->snapdir);
	if (smb_fname.base_name == NULL) {
		return NULL;
	}

	ret = SMB_VFS_NEXT_STAT(handle, &smb_fname);
	if ((ret == 0) && (S_ISDIR(smb_fname.st.st_ex_mode))) {
		return smb_fname.base_name;
	}
	TALLOC_FREE(smb_fname.base_name);
	return NULL;
}