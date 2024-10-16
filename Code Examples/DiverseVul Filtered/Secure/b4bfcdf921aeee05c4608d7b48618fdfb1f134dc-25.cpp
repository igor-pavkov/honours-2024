static NTSTATUS copy_internals(TALLOC_CTX *ctx,
				connection_struct *conn,
				struct smb_request *req,
				struct smb_filename *smb_fname_src,
				struct smb_filename *smb_fname_dst,
				uint32 attrs)
{
	files_struct *fsp1,*fsp2;
	uint32 fattr;
	int info;
	off_t ret=-1;
	NTSTATUS status = NT_STATUS_OK;
	char *parent;

	if (!CAN_WRITE(conn)) {
		status = NT_STATUS_MEDIA_WRITE_PROTECTED;
		goto out;
	}

        /* Source must already exist. */
	if (!VALID_STAT(smb_fname_src->st)) {
		status = NT_STATUS_OBJECT_NAME_NOT_FOUND;
		goto out;
	}

	/* Ensure attributes match. */
	fattr = dos_mode(conn, smb_fname_src);
	if ((fattr & ~attrs) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
		status = NT_STATUS_NO_SUCH_FILE;
		goto out;
	}

	/* Disallow if dst file already exists. */
	if (VALID_STAT(smb_fname_dst->st)) {
		status = NT_STATUS_OBJECT_NAME_COLLISION;
		goto out;
	}

	/* No links from a directory. */
	if (S_ISDIR(smb_fname_src->st.st_ex_mode)) {
		status = NT_STATUS_FILE_IS_A_DIRECTORY;
		goto out;
	}

	DEBUG(10,("copy_internals: doing file copy %s to %s\n",
		  smb_fname_str_dbg(smb_fname_src),
		  smb_fname_str_dbg(smb_fname_dst)));

        status = SMB_VFS_CREATE_FILE(
		conn,					/* conn */
		req,					/* req */
		0,					/* root_dir_fid */
		smb_fname_src,				/* fname */
		FILE_READ_DATA|FILE_READ_ATTRIBUTES|
			FILE_READ_EA,			/* access_mask */
		(FILE_SHARE_READ | FILE_SHARE_WRITE |	/* share_access */
		    FILE_SHARE_DELETE),
		FILE_OPEN,				/* create_disposition*/
		0,					/* create_options */
		FILE_ATTRIBUTE_NORMAL,			/* file_attributes */
		NO_OPLOCK,				/* oplock_request */
		0,					/* allocation_size */
		0,					/* private_flags */
		NULL,					/* sd */
		NULL,					/* ea_list */
		&fsp1,					/* result */
		&info);					/* pinfo */

	if (!NT_STATUS_IS_OK(status)) {
		goto out;
	}

        status = SMB_VFS_CREATE_FILE(
		conn,					/* conn */
		req,					/* req */
		0,					/* root_dir_fid */
		smb_fname_dst,				/* fname */
		FILE_WRITE_DATA|FILE_WRITE_ATTRIBUTES|
			FILE_WRITE_EA,			/* access_mask */
		(FILE_SHARE_READ | FILE_SHARE_WRITE |	/* share_access */
		    FILE_SHARE_DELETE),
		FILE_CREATE,				/* create_disposition*/
		0,					/* create_options */
		fattr,					/* file_attributes */
		NO_OPLOCK,				/* oplock_request */
		0,					/* allocation_size */
		0,					/* private_flags */
		NULL,					/* sd */
		NULL,					/* ea_list */
		&fsp2,					/* result */
		&info);					/* pinfo */

	if (!NT_STATUS_IS_OK(status)) {
		close_file(NULL, fsp1, ERROR_CLOSE);
		goto out;
	}

	if (smb_fname_src->st.st_ex_size) {
		ret = vfs_transfer_file(fsp1, fsp2, smb_fname_src->st.st_ex_size);
	}

	/*
	 * As we are opening fsp1 read-only we only expect
	 * an error on close on fsp2 if we are out of space.
	 * Thus we don't look at the error return from the
	 * close of fsp1.
	 */
	close_file(NULL, fsp1, NORMAL_CLOSE);

	/* Ensure the modtime is set correctly on the destination file. */
	set_close_write_time(fsp2, smb_fname_src->st.st_ex_mtime);

	status = close_file(NULL, fsp2, NORMAL_CLOSE);

	/* Grrr. We have to do this as open_file_ntcreate adds FILE_ATTRIBUTE_ARCHIVE when it
	   creates the file. This isn't the correct thing to do in the copy
	   case. JRA */
	if (!parent_dirname(talloc_tos(), smb_fname_dst->base_name, &parent,
			    NULL)) {
		status = NT_STATUS_NO_MEMORY;
		goto out;
	}
	file_set_dosmode(conn, smb_fname_dst, fattr, parent, false);
	TALLOC_FREE(parent);

	if (ret < (off_t)smb_fname_src->st.st_ex_size) {
		status = NT_STATUS_DISK_FULL;
		goto out;
	}
 out:
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(3,("copy_internals: Error %s copy file %s to %s\n",
			nt_errstr(status), smb_fname_str_dbg(smb_fname_src),
			smb_fname_str_dbg(smb_fname_dst)));
	}

	return status;
}