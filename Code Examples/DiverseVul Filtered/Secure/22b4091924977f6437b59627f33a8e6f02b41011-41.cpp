NTSTATUS get_ea_value_fsp(TALLOC_CTX *mem_ctx,
			  files_struct *fsp,
			  const char *ea_name,
			  struct ea_struct *pea)
{
	/* Get the value of this xattr. Max size is 64k. */
	size_t attr_size = 256;
	char *val = NULL;
	ssize_t sizeret;
	size_t max_xattr_size = 0;

	if (fsp == NULL) {
		return NT_STATUS_INVALID_HANDLE;
	}

	max_xattr_size = lp_smbd_max_xattr_size(SNUM(fsp->conn));

 again:

	val = talloc_realloc(mem_ctx, val, char, attr_size);
	if (!val) {
		return NT_STATUS_NO_MEMORY;
	}

	sizeret = SMB_VFS_FGETXATTR(fsp, ea_name, val, attr_size);
	if (sizeret == -1 && errno == ERANGE && attr_size < max_xattr_size) {
		attr_size = max_xattr_size;
		goto again;
	}

	if (sizeret == -1) {
		return map_nt_error_from_unix(errno);
	}

	DEBUG(10,("get_ea_value: EA %s is of length %u\n", ea_name, (unsigned int)sizeret));
	dump_data(10, (uint8_t *)val, sizeret);

	pea->flags = 0;
	if (strnequal(ea_name, "user.", 5)) {
		pea->name = talloc_strdup(mem_ctx, &ea_name[5]);
	} else {
		pea->name = talloc_strdup(mem_ctx, ea_name);
	}
	if (pea->name == NULL) {
		TALLOC_FREE(val);
		return NT_STATUS_NO_MEMORY;
	}
	pea->value.data = (unsigned char *)val;
	pea->value.length = (size_t)sizeret;
	return NT_STATUS_OK;
}