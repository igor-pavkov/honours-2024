NTSTATUS set_sd_blob(files_struct *fsp, uint8_t *data, uint32_t sd_len,
		       uint32_t security_info_sent)
{
	struct security_descriptor *psd = NULL;
	NTSTATUS status;

	if (sd_len == 0) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	status = unmarshall_sec_desc(talloc_tos(), data, sd_len, &psd);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	return set_sd(fsp, psd, security_info_sent);
}