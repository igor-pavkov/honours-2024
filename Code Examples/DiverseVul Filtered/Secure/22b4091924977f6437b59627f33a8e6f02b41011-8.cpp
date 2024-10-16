static bool smbd_dirptr_lanman2_mode_fn(TALLOC_CTX *ctx,
					void *private_data,
					struct files_struct *dirfsp,
					struct smb_filename *atname,
					struct smb_filename *smb_fname,
					bool get_dosmode,
					uint32_t *_mode)
{
	struct smbd_dirptr_lanman2_state *state =
		(struct smbd_dirptr_lanman2_state *)private_data;
	bool ms_dfs_link = false;

	if (smb_fname->flags & SMB_FILENAME_POSIX_PATH) {
		if (SMB_VFS_LSTAT(state->conn, smb_fname) != 0) {
			DEBUG(5,("smbd_dirptr_lanman2_mode_fn: "
				 "Couldn't lstat [%s] (%s)\n",
				 smb_fname_str_dbg(smb_fname),
				 strerror(errno)));
			return false;
		}
		return true;
	} else if (!VALID_STAT(smb_fname->st) &&
		   SMB_VFS_STAT(state->conn, smb_fname) != 0) {
		/* Needed to show the msdfs symlinks as
		 * directories */

		ms_dfs_link = check_msdfs_link(dirfsp,
					       atname,
					       smb_fname);
		if (!ms_dfs_link) {
			DEBUG(5,("smbd_dirptr_lanman2_mode_fn: "
				 "Couldn't stat [%s] (%s)\n",
				 smb_fname_str_dbg(smb_fname),
				 strerror(errno)));
			return false;
		}

		*_mode = dos_mode_msdfs(state->conn, smb_fname);
		return true;
	}

	if (!get_dosmode) {
		return true;
	}

	*_mode = fdos_mode(smb_fname->fsp);
	smb_fname->st = smb_fname->fsp->fsp_name->st;

	return true;
}