static NTSTATUS vfswrap_streaminfo(vfs_handle_struct *handle,
				   struct files_struct *fsp,
				   const char *fname,
				   TALLOC_CTX *mem_ctx,
				   unsigned int *pnum_streams,
				   struct stream_struct **pstreams)
{
	SMB_STRUCT_STAT sbuf;
	struct stream_struct *tmp_streams = NULL;
	int ret;

	if ((fsp != NULL) && (fsp->is_directory)) {
		/*
		 * No default streams on directories
		 */
		goto done;
	}

	if ((fsp != NULL) && (fsp->fh->fd != -1)) {
		ret = SMB_VFS_FSTAT(fsp, &sbuf);
	}
	else {
		struct smb_filename smb_fname;

		ZERO_STRUCT(smb_fname);
		smb_fname.base_name = discard_const_p(char, fname);

		if (lp_posix_pathnames()) {
			ret = SMB_VFS_LSTAT(handle->conn, &smb_fname);
		} else {
			ret = SMB_VFS_STAT(handle->conn, &smb_fname);
		}
		sbuf = smb_fname.st;
	}

	if (ret == -1) {
		return map_nt_error_from_unix(errno);
	}

	if (S_ISDIR(sbuf.st_ex_mode)) {
		goto done;
	}

	tmp_streams = talloc_realloc(mem_ctx, *pstreams, struct stream_struct,
					(*pnum_streams) + 1);
	if (tmp_streams == NULL) {
		return NT_STATUS_NO_MEMORY;
	}
	tmp_streams[*pnum_streams].name = talloc_strdup(tmp_streams, "::$DATA");
	if (tmp_streams[*pnum_streams].name == NULL) {
		return NT_STATUS_NO_MEMORY;
	}
	tmp_streams[*pnum_streams].size = sbuf.st_ex_size;
	tmp_streams[*pnum_streams].alloc_size = SMB_VFS_GET_ALLOC_SIZE(handle->conn, fsp, &sbuf);

	*pnum_streams += 1;
	*pstreams = tmp_streams;
 done:
	return NT_STATUS_OK;
}