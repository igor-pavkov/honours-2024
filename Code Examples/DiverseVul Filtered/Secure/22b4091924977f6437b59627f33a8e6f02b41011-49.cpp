NTSTATUS smbd_dirptr_lanman2_entry(TALLOC_CTX *ctx,
			       connection_struct *conn,
			       struct dptr_struct *dirptr,
			       uint16_t flags2,
			       const char *path_mask,
			       uint32_t dirtype,
			       int info_level,
			       int requires_resume_key,
			       bool dont_descend,
			       bool ask_sharemode,
			       bool get_dosmode,
			       uint8_t align,
			       bool do_pad,
			       char **ppdata,
			       char *base_data,
			       char *end_data,
			       int space_remaining,
			       struct smb_filename **_smb_fname,
			       bool *got_exact_match,
			       int *_last_entry_off,
			       struct ea_list *name_list,
			       struct file_id *file_id)
{
	const char *p;
	const char *mask = NULL;
	long prev_dirpos = 0;
	uint32_t mode = 0;
	char *fname = NULL;
	struct smb_filename *smb_fname = NULL;
	struct smbd_dirptr_lanman2_state state;
	bool ok;
	uint64_t last_entry_off = 0;
	NTSTATUS status;
	enum mangled_names_options mangled_names;
	bool marshall_with_83_names;

	mangled_names = lp_mangled_names(conn->params);

	ZERO_STRUCT(state);
	state.conn = conn;
	state.info_level = info_level;
	if (mangled_names != MANGLED_NAMES_NO) {
		state.check_mangled_names = true;
	}
	state.has_wild = dptr_has_wild(dirptr);
	state.got_exact_match = false;

	*got_exact_match = false;

	p = strrchr_m(path_mask,'/');
	if(p != NULL) {
		if(p[1] == '\0') {
			mask = "*.*";
		} else {
			mask = p+1;
		}
	} else {
		mask = path_mask;
	}

	ok = smbd_dirptr_get_entry(ctx,
				   dirptr,
				   mask,
				   dirtype,
				   dont_descend,
				   ask_sharemode,
				   get_dosmode,
				   smbd_dirptr_lanman2_match_fn,
				   smbd_dirptr_lanman2_mode_fn,
				   &state,
				   &fname,
				   &smb_fname,
				   &mode,
				   &prev_dirpos);
	if (!ok) {
		return NT_STATUS_END_OF_FILE;
	}

	*got_exact_match = state.got_exact_match;

	marshall_with_83_names = (mangled_names == MANGLED_NAMES_YES);

	status = smbd_marshall_dir_entry(ctx,
				     conn,
				     flags2,
				     info_level,
				     name_list,
				     marshall_with_83_names,
				     requires_resume_key,
				     mode,
				     fname,
				     smb_fname,
				     space_remaining,
				     align,
				     do_pad,
				     base_data,
				     ppdata,
				     end_data,
				     &last_entry_off);
	if (NT_STATUS_EQUAL(status, NT_STATUS_ILLEGAL_CHARACTER)) {
		DEBUG(1,("Conversion error: illegal character: %s\n",
			 smb_fname_str_dbg(smb_fname)));
	}

	if (file_id != NULL) {
		*file_id = vfs_file_id_from_sbuf(conn, &smb_fname->st);
	}

	if (!NT_STATUS_IS_OK(status) &&
	    !NT_STATUS_EQUAL(status, STATUS_MORE_ENTRIES))
	{
		TALLOC_FREE(smb_fname);
		TALLOC_FREE(fname);
		return status;
	}

	if (_smb_fname != NULL) {
		/*
		 * smb_fname is already talloc'ed off ctx.
		 * We just need to make sure we don't return
		 * any stream_name, and replace base_name
		 * with fname in case base_name got mangled.
		 * This allows us to preserve any smb_fname->fsp
		 * for asynchronous handle lookups.
		 */
		TALLOC_FREE(smb_fname->stream_name);
		TALLOC_FREE(smb_fname->base_name);
		smb_fname->base_name = talloc_strdup(smb_fname, fname);

		if (smb_fname->base_name == NULL) {
			TALLOC_FREE(smb_fname);
			TALLOC_FREE(fname);
			return NT_STATUS_NO_MEMORY;
		}
		*_smb_fname = smb_fname;
	} else {
		TALLOC_FREE(smb_fname);
	}
	TALLOC_FREE(fname);

	if (NT_STATUS_EQUAL(status, STATUS_MORE_ENTRIES)) {
		dptr_SeekDir(dirptr, prev_dirpos);
		return status;
	}

	*_last_entry_off = last_entry_off;
	return NT_STATUS_OK;
}