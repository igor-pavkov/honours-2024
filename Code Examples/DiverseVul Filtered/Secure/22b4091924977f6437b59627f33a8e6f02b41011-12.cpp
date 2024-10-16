NTSTATUS check_access_fsp(struct files_struct *fsp,
			  uint32_t access_mask)
{
	if (!fsp->fsp_flags.is_fsa) {
		return smbd_check_access_rights_fsp(fsp->conn->cwd_fsp,
						    fsp,
						    false,
						    access_mask);
	}
	if (!(fsp->access_mask & access_mask)) {
		return NT_STATUS_ACCESS_DENIED;
	}
	return NT_STATUS_OK;
}