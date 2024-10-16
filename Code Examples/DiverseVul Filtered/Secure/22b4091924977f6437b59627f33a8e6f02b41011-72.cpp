static unsigned int estimate_ea_size(files_struct *fsp)
{
	size_t total_ea_len = 0;
	TALLOC_CTX *mem_ctx;
	struct ea_list *ea_list = NULL;
	NTSTATUS status;

	/* symlink */
	if (fsp == NULL) {
		return 0;
	}

	if (!lp_ea_support(SNUM(fsp->conn))) {
		return 0;
	}

	mem_ctx = talloc_stackframe();

	/* If this is a stream fsp, then we need to instead find the
	 * estimated ea len from the main file, not the stream
	 * (streams cannot have EAs), but the estimate isn't just 0 in
	 * this case! */
	if (is_ntfs_stream_smb_fname(fsp->fsp_name)) {
		fsp = fsp->base_fsp;
	}
	(void)get_ea_list_from_fsp(mem_ctx,
				   fsp,
				   &total_ea_len,
				   &ea_list);

	if(fsp->conn->sconn->using_smb2) {
		unsigned int ret_data_size;
		/*
		 * We're going to be using fill_ea_chained_buffer() to
		 * marshall EA's - this size is significantly larger
		 * than the SMB1 buffer. Re-calculate the size without
		 * marshalling.
		 */
		status = fill_ea_chained_buffer(mem_ctx,
						NULL,
						0,
						&ret_data_size,
						fsp->conn,
						ea_list);
		if (!NT_STATUS_IS_OK(status)) {
			ret_data_size = 0;
		}
		total_ea_len = ret_data_size;
	}
	TALLOC_FREE(mem_ctx);
	return total_ea_len;
}