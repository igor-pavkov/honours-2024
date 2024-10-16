NTSTATUS get_ea_names_from_fsp(TALLOC_CTX *mem_ctx,
				files_struct *fsp,
				char ***pnames,
				size_t *pnum_names)
{
	char smallbuf[1024];
	/* Get a list of all xattrs. Max namesize is 64k. */
	size_t ea_namelist_size = 1024;
	char *ea_namelist = smallbuf;
	char *to_free = NULL;

	char *p;
	char **names;
	size_t num_names;
	ssize_t sizeret = -1;
	NTSTATUS status;

	if (pnames) {
		*pnames = NULL;
	}
	*pnum_names = 0;

	if (fsp == NULL) {
		/*
		 * Callers may pass fsp == NULL when passing smb_fname->fsp of a
		 * symlink. This is ok, handle it here, by just return no EA's
		 * on a symlink.
		 */
		return NT_STATUS_OK;
	}

	/* should be the case that fsp != NULL */
	SMB_ASSERT(fsp != NULL);

	sizeret = SMB_VFS_FLISTXATTR(fsp, ea_namelist,
				     ea_namelist_size);

	if ((sizeret == -1) && (errno == ERANGE)) {
		ea_namelist_size = 65536;
		ea_namelist = talloc_array(mem_ctx, char, ea_namelist_size);
		if (ea_namelist == NULL) {
			return NT_STATUS_NO_MEMORY;
		}
		to_free = ea_namelist;

		sizeret = SMB_VFS_FLISTXATTR(fsp, ea_namelist,
					     ea_namelist_size);
	}

	if (sizeret == -1) {
		status = map_nt_error_from_unix(errno);
		TALLOC_FREE(to_free);
		return status;
	}

	DBG_DEBUG("ea_namelist size = %zd\n", sizeret);

	if (sizeret == 0) {
		TALLOC_FREE(to_free);
		return NT_STATUS_OK;
	}

	/*
	 * Ensure the result is 0-terminated
	 */

	if (ea_namelist[sizeret-1] != '\0') {
		TALLOC_FREE(to_free);
		return NT_STATUS_INTERNAL_ERROR;
	}

	/*
	 * count the names
	 */
	num_names = 0;

	for (p = ea_namelist; p - ea_namelist < sizeret; p += strlen(p)+1) {
		num_names += 1;
	}

	*pnum_names = num_names;

	if (pnames == NULL) {
		TALLOC_FREE(to_free);
		return NT_STATUS_OK;
	}

	names = talloc_array(mem_ctx, char *, num_names);
	if (names == NULL) {
		DEBUG(0, ("talloc failed\n"));
		TALLOC_FREE(to_free);
		return NT_STATUS_NO_MEMORY;
	}

	if (ea_namelist == smallbuf) {
		ea_namelist = talloc_memdup(names, smallbuf, sizeret);
		if (ea_namelist == NULL) {
			TALLOC_FREE(names);
			return NT_STATUS_NO_MEMORY;
		}
	} else {
		talloc_steal(names, ea_namelist);

		ea_namelist = talloc_realloc(names, ea_namelist, char,
					     sizeret);
		if (ea_namelist == NULL) {
			TALLOC_FREE(names);
			return NT_STATUS_NO_MEMORY;
		}
	}

	num_names = 0;

	for (p = ea_namelist; p - ea_namelist < sizeret; p += strlen(p)+1) {
		names[num_names++] = p;
	}

	*pnames = names;

	return NT_STATUS_OK;
}