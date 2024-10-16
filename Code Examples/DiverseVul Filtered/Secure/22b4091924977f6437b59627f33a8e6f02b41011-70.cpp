static NTSTATUS smb_unix_mknod(connection_struct *conn,
					const char *pdata,
					int total_data,
					const struct smb_filename *smb_fname)
{
	uint32_t file_type = IVAL(pdata,56);
#if defined(HAVE_MAKEDEV)
	uint32_t dev_major = IVAL(pdata,60);
	uint32_t dev_minor = IVAL(pdata,68);
#endif
	SMB_DEV_T dev = (SMB_DEV_T)0;
	uint32_t raw_unixmode = IVAL(pdata,84);
	NTSTATUS status;
	mode_t unixmode;
	int ret;
	struct smb_filename *parent_fname = NULL;
	struct smb_filename *base_name = NULL;

	if (total_data < 100) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	status = unix_perms_from_wire(conn, &smb_fname->st, raw_unixmode,
				      PERM_NEW_FILE, &unixmode);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

#if defined(HAVE_MAKEDEV)
	dev = makedev(dev_major, dev_minor);
#endif

	switch (file_type) {
		/* We can't create other objects here. */
		case UNIX_TYPE_FILE:
		case UNIX_TYPE_DIR:
		case UNIX_TYPE_SYMLINK:
			return NT_STATUS_ACCESS_DENIED;
#if defined(S_IFIFO)
		case UNIX_TYPE_FIFO:
			unixmode |= S_IFIFO;
			break;
#endif
#if defined(S_IFSOCK)
		case UNIX_TYPE_SOCKET:
			unixmode |= S_IFSOCK;
			break;
#endif
#if defined(S_IFCHR)
		case UNIX_TYPE_CHARDEV:
			/* This is only allowed for root. */
			if (get_current_uid(conn) != sec_initial_uid()) {
				return NT_STATUS_ACCESS_DENIED;
			}
			unixmode |= S_IFCHR;
			break;
#endif
#if defined(S_IFBLK)
		case UNIX_TYPE_BLKDEV:
			if (get_current_uid(conn) != sec_initial_uid()) {
				return NT_STATUS_ACCESS_DENIED;
			}
			unixmode |= S_IFBLK;
			break;
#endif
		default:
			return NT_STATUS_INVALID_PARAMETER;
	}

	DEBUG(10,("smb_unix_mknod: SMB_SET_FILE_UNIX_BASIC doing mknod dev "
		  "%.0f mode 0%o for file %s\n", (double)dev,
		  (unsigned int)unixmode, smb_fname_str_dbg(smb_fname)));

	status = parent_pathref(talloc_tos(),
				conn->cwd_fsp,
				smb_fname,
				&parent_fname,
				&base_name);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/* Ok - do the mknod. */
	ret = SMB_VFS_MKNODAT(conn,
			parent_fname->fsp,
			base_name,
			unixmode,
			dev);

	if (ret != 0) {
		TALLOC_FREE(parent_fname);
		return map_nt_error_from_unix(errno);
	}

	/* If any of the other "set" calls fail we
 	 * don't want to end up with a half-constructed mknod.
 	 */

	if (lp_inherit_permissions(SNUM(conn))) {
		inherit_access_posix_acl(conn,
					 parent_fname,
					 smb_fname,
					 unixmode);
	}
	TALLOC_FREE(parent_fname);

	return NT_STATUS_OK;
}