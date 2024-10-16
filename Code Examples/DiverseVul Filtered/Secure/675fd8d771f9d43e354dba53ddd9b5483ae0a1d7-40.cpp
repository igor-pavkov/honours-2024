static bool check_access_snapdir(struct vfs_handle_struct *handle,
				const char *path)
{
	struct smb_filename smb_fname;
	int ret;
	NTSTATUS status;

	ZERO_STRUCT(smb_fname);
	smb_fname.base_name = talloc_asprintf(talloc_tos(),
						"%s",
						path);
	if (smb_fname.base_name == NULL) {
		return false;
	}

	ret = SMB_VFS_NEXT_STAT(handle, &smb_fname);
	if (ret != 0 || !S_ISDIR(smb_fname.st.st_ex_mode)) {
		TALLOC_FREE(smb_fname.base_name);
		return false;
	}

	status = smbd_check_access_rights(handle->conn,
					&smb_fname,
					false,
					SEC_DIR_LIST);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0,("user does not have list permission "
			"on snapdir %s\n",
			smb_fname.base_name));
		TALLOC_FREE(smb_fname.base_name);
		return false;
	}
	TALLOC_FREE(smb_fname.base_name);
	return true;
}