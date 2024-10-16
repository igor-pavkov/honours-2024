static NTSTATUS smb_set_file_dosmode(connection_struct *conn,
				     struct files_struct *fsp,
				     uint32_t dosmode)
{
	struct files_struct *dos_fsp = NULL;
	uint32_t current_dosmode;
	int ret;

	if (!VALID_STAT(fsp->fsp_name->st)) {
		return NT_STATUS_OBJECT_NAME_NOT_FOUND;
	}

	dos_fsp = fsp->base_fsp != NULL ? fsp->base_fsp : fsp;

	if (dosmode != 0) {
		if (S_ISDIR(fsp->fsp_name->st.st_ex_mode)) {
			dosmode |= FILE_ATTRIBUTE_DIRECTORY;
		} else {
			dosmode &= ~FILE_ATTRIBUTE_DIRECTORY;
		}
	}

	DBG_DEBUG("dosmode: 0x%" PRIx32 "\n", dosmode);

	/* check the mode isn't different, before changing it */
	if (dosmode == 0) {
		return NT_STATUS_OK;
	}
	current_dosmode = fdos_mode(dos_fsp);
	if (dosmode == current_dosmode) {
		return NT_STATUS_OK;
	}

	DBG_DEBUG("file %s : setting dos mode 0x%" PRIx32 "\n",
		  fsp_str_dbg(dos_fsp), dosmode);

	ret = file_set_dosmode(conn, dos_fsp->fsp_name, dosmode, NULL, false);
	if (ret != 0) {
		DBG_WARNING("file_set_dosmode of %s failed: %s\n",
			    fsp_str_dbg(dos_fsp), strerror(errno));
		return map_nt_error_from_unix(errno);
	}

	return NT_STATUS_OK;
}