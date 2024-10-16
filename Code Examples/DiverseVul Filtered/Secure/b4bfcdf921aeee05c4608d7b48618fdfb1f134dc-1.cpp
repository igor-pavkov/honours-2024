static NTSTATUS get_null_nt_acl(TALLOC_CTX *mem_ctx, struct security_descriptor **ppsd)
{
	size_t sd_size;

	*ppsd = make_standard_sec_desc( mem_ctx, &global_sid_World, &global_sid_World, NULL, &sd_size);
	if(!*ppsd) {
		DEBUG(0,("get_null_nt_acl: Unable to malloc space for security descriptor.\n"));
		return NT_STATUS_NO_MEMORY;
	}

	return NT_STATUS_OK;
}