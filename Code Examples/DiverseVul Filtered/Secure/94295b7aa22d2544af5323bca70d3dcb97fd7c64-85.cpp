static NTSTATUS smb_bytes_talloc_string(TALLOC_CTX *mem_ctx,
					const uint8_t *hdr,
					char **dest,
					uint8_t *src,
					size_t srclen,
					ssize_t *destlen)
{
	*destlen = clistr_pull_talloc(mem_ctx,
				(const char *)hdr,
				SVAL(hdr, HDR_FLG2),
				dest,
				(char *)src,
				srclen,
				STR_TERMINATE);
	if (*destlen == -1) {
		return NT_STATUS_NO_MEMORY;
	}

	if (*dest == NULL) {
		*dest = talloc_strdup(mem_ctx, "");
		if (*dest == NULL) {
			return NT_STATUS_NO_MEMORY;
		}
	}
	return NT_STATUS_OK;
}