NTSTATUS refuse_symlink_fsp(const files_struct *fsp)
{

	if (!VALID_STAT(fsp->fsp_name->st)) {
		return NT_STATUS_ACCESS_DENIED;
	}
	if (S_ISLNK(fsp->fsp_name->st.st_ex_mode)) {
		return NT_STATUS_ACCESS_DENIED;
	}
	if (fsp_get_pathref_fd(fsp) == -1) {
		return NT_STATUS_ACCESS_DENIED;
	}
	return NT_STATUS_OK;
}