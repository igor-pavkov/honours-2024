static NTSTATUS smb_set_file_unix_basic(connection_struct *conn,
					struct smb_request *req,
					const char *pdata,
					int total_data,
					files_struct *fsp,
					struct smb_filename *smb_fname)
{
	struct smb_file_time ft;
	uint32_t raw_unixmode;
	mode_t unixmode;
	off_t size = 0;
	uid_t set_owner = (uid_t)SMB_UID_NO_CHANGE;
	gid_t set_grp = (uid_t)SMB_GID_NO_CHANGE;
	NTSTATUS status = NT_STATUS_OK;
	enum perm_type ptype;
	files_struct *all_fsps = NULL;
	bool modify_mtime = true;
	struct file_id id;
	SMB_STRUCT_STAT sbuf;

	init_smb_file_time(&ft);

	if (total_data < 100) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if(IVAL(pdata, 0) != SMB_SIZE_NO_CHANGE_LO &&
	   IVAL(pdata, 4) != SMB_SIZE_NO_CHANGE_HI) {
		size=IVAL(pdata,0); /* first 8 Bytes are size */
		size |= (((off_t)IVAL(pdata,4)) << 32);
	}

	ft.atime = pull_long_date_full_timespec(pdata+24); /* access_time */
	ft.mtime = pull_long_date_full_timespec(pdata+32); /* modification_time */
	set_owner = (uid_t)IVAL(pdata,40);
	set_grp = (gid_t)IVAL(pdata,48);
	raw_unixmode = IVAL(pdata,84);

	if (VALID_STAT(smb_fname->st)) {
		if (S_ISDIR(smb_fname->st.st_ex_mode)) {
			ptype = PERM_EXISTING_DIR;
		} else {
			ptype = PERM_EXISTING_FILE;
		}
	} else {
		ptype = PERM_NEW_FILE;
	}

	status = unix_perms_from_wire(conn, &smb_fname->st, raw_unixmode,
				      ptype, &unixmode);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	DEBUG(10,("smb_set_file_unix_basic: SMB_SET_FILE_UNIX_BASIC: name = "
		  "%s size = %.0f, uid = %u, gid = %u, raw perms = 0%o\n",
		  smb_fname_str_dbg(smb_fname), (double)size,
		  (unsigned int)set_owner, (unsigned int)set_grp,
		  (int)raw_unixmode));

	sbuf = smb_fname->st;

	if (!VALID_STAT(sbuf)) {
		/*
		 * The only valid use of this is to create character and block
		 * devices, and named pipes. This is deprecated (IMHO) and 
		 * a new info level should be used for mknod. JRA.
		 */

		return smb_unix_mknod(conn,
					pdata,
					total_data,
					smb_fname);
	}

#if 1
	/* Horrible backwards compatibility hack as an old server bug
	 * allowed a CIFS client bug to remain unnoticed :-(. JRA.
	 * */

	if (!size) {
		size = get_file_size_stat(&sbuf);
	}
#endif

	/*
	 * Deal with the UNIX specific mode set.
	 */

	if (raw_unixmode != SMB_MODE_NO_CHANGE) {
		int ret;

		if (fsp == NULL || S_ISLNK(smb_fname->st.st_ex_mode)) {
			DBG_WARNING("Can't set mode on symlink %s\n",
				smb_fname_str_dbg(smb_fname));
			return NT_STATUS_OBJECT_NAME_NOT_FOUND;
		}

		DEBUG(10,("smb_set_file_unix_basic: SMB_SET_FILE_UNIX_BASIC "
			  "setting mode 0%o for file %s\n",
			  (unsigned int)unixmode,
			  smb_fname_str_dbg(smb_fname)));
		ret = SMB_VFS_FCHMOD(fsp, unixmode);
		if (ret != 0) {
			return map_nt_error_from_unix(errno);
		}
	}

	/*
	 * Deal with the UNIX specific uid set.
	 */

	if ((set_owner != (uid_t)SMB_UID_NO_CHANGE) &&
	    (sbuf.st_ex_uid != set_owner)) {
		int ret;

		DEBUG(10,("smb_set_file_unix_basic: SMB_SET_FILE_UNIX_BASIC "
			  "changing owner %u for path %s\n",
			  (unsigned int)set_owner,
			  smb_fname_str_dbg(smb_fname)));

		if (fsp &&
		    !fsp->fsp_flags.is_pathref &&
		    fsp_get_io_fd(fsp) != -1)
		{
			ret = SMB_VFS_FCHOWN(fsp, set_owner, (gid_t)-1);
		} else {
			/*
			 * UNIX extensions calls must always operate
			 * on symlinks.
			 */
			ret = SMB_VFS_LCHOWN(conn, smb_fname,
					     set_owner, (gid_t)-1);
		}

		if (ret != 0) {
			status = map_nt_error_from_unix(errno);
			return status;
		}
	}

	/*
	 * Deal with the UNIX specific gid set.
	 */

	if ((set_grp != (uid_t)SMB_GID_NO_CHANGE) &&
	    (sbuf.st_ex_gid != set_grp)) {
		int ret;

		DEBUG(10,("smb_set_file_unix_basic: SMB_SET_FILE_UNIX_BASIC "
			  "changing group %u for file %s\n",
			  (unsigned int)set_grp,
			  smb_fname_str_dbg(smb_fname)));
		if (fsp &&
		    !fsp->fsp_flags.is_pathref &&
		    fsp_get_io_fd(fsp) != -1)
		{
			ret = SMB_VFS_FCHOWN(fsp, (uid_t)-1, set_grp);
		} else {
			/*
			 * UNIX extensions calls must always operate
			 * on symlinks.
			 */
			ret = SMB_VFS_LCHOWN(conn, smb_fname, (uid_t)-1,
				  set_grp);
		}
		if (ret != 0) {
			status = map_nt_error_from_unix(errno);
			return status;
		}
	}

	/* Deal with any size changes. */

	if (S_ISREG(sbuf.st_ex_mode)) {
		status = smb_set_file_size(conn, req,
					   fsp,
					   smb_fname,
					   &sbuf,
					   size,
					   false);
		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}
	}

	/* Deal with any time changes. */
	if (is_omit_timespec(&ft.mtime) && is_omit_timespec(&ft.atime)) {
		/* No change, don't cancel anything. */
		return status;
	}

	id = vfs_file_id_from_sbuf(conn, &sbuf);
	for(all_fsps = file_find_di_first(conn->sconn, id, true); all_fsps;
			all_fsps = file_find_di_next(all_fsps, true)) {
		/*
		 * We're setting the time explicitly for UNIX.
		 * Cancel any pending changes over all handles.
		 */
		all_fsps->fsp_flags.update_write_time_on_close = false;
		TALLOC_FREE(all_fsps->update_write_time_event);
	}

	/*
	 * Override the "setting_write_time"
	 * parameter here as it almost does what
	 * we need. Just remember if we modified
	 * mtime and send the notify ourselves.
	 */
	if (is_omit_timespec(&ft.mtime)) {
		modify_mtime = false;
	}

	status = smb_set_file_time(conn,
				fsp,
				smb_fname,
				&ft,
				false);
	if (modify_mtime) {
		notify_fname(conn, NOTIFY_ACTION_MODIFIED,
			FILE_NOTIFY_CHANGE_LAST_WRITE, smb_fname->base_name);
	}
	return status;
}