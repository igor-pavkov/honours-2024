NTSTATUS smbd_do_qfilepathinfo(connection_struct *conn,
			       TALLOC_CTX *mem_ctx,
			       struct smb_request *req,
			       uint16_t info_level,
			       files_struct *fsp,
			       struct smb_filename *smb_fname,
			       bool delete_pending,
			       struct timespec write_time_ts,
			       struct ea_list *ea_list,
			       int lock_data_count,
			       char *lock_data,
			       uint16_t flags2,
			       unsigned int max_data_bytes,
			       size_t *fixed_portion,
			       char **ppdata,
			       unsigned int *pdata_size)
{
	char *pdata = *ppdata;
	char *dstart, *dend;
	unsigned int data_size;
	struct timespec create_time_ts, mtime_ts, atime_ts, ctime_ts;
	time_t create_time, mtime, atime, c_time;
	SMB_STRUCT_STAT *psbuf = NULL;
	char *p;
	char *base_name;
	char *dos_fname;
	int mode;
	int nlink;
	NTSTATUS status;
	uint64_t file_size = 0;
	uint64_t pos = 0;
	uint64_t allocation_size = 0;
	uint64_t file_id = 0;
	uint32_t access_mask = 0;
	size_t len = 0;

	if (INFO_LEVEL_IS_UNIX(info_level) && !lp_unix_extensions()) {
		return NT_STATUS_INVALID_LEVEL;
	}

	DEBUG(5,("smbd_do_qfilepathinfo: %s (%s) level=%d max_data=%u\n",
		 smb_fname_str_dbg(smb_fname),
		 fsp_fnum_dbg(fsp),
		 info_level, max_data_bytes));

	/*
	 * In case of querying a symlink in POSIX context,
	 * fsp will be NULL. fdos_mode() deals with it.
	 */
	if (fsp != NULL) {
		smb_fname = fsp->fsp_name;
	}
	mode = fdos_mode(fsp);
	psbuf = &smb_fname->st;

	nlink = psbuf->st_ex_nlink;

	if (nlink && (mode&FILE_ATTRIBUTE_DIRECTORY)) {
		nlink = 1;
	}

	if ((nlink > 0) && delete_pending) {
		nlink -= 1;
	}

	if (max_data_bytes + DIR_ENTRY_SAFETY_MARGIN < max_data_bytes) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	data_size = max_data_bytes + DIR_ENTRY_SAFETY_MARGIN;
	*ppdata = (char *)SMB_REALLOC(*ppdata, data_size); 
	if (*ppdata == NULL) {
		return NT_STATUS_NO_MEMORY;
	}
	pdata = *ppdata;
	dstart = pdata;
	dend = dstart + data_size - 1;

	if (!is_omit_timespec(&write_time_ts) &&
	    !INFO_LEVEL_IS_UNIX(info_level))
	{
		update_stat_ex_mtime(psbuf, write_time_ts);
	}

	create_time_ts = get_create_timespec(conn, fsp, smb_fname);
	mtime_ts = psbuf->st_ex_mtime;
	atime_ts = psbuf->st_ex_atime;
	ctime_ts = get_change_timespec(conn, fsp, smb_fname);

	if (lp_dos_filetime_resolution(SNUM(conn))) {
		dos_filetime_timespec(&create_time_ts);
		dos_filetime_timespec(&mtime_ts);
		dos_filetime_timespec(&atime_ts);
		dos_filetime_timespec(&ctime_ts);
	}

	create_time = convert_timespec_to_time_t(create_time_ts);
	mtime = convert_timespec_to_time_t(mtime_ts);
	atime = convert_timespec_to_time_t(atime_ts);
	c_time = convert_timespec_to_time_t(ctime_ts);

	p = strrchr_m(smb_fname->base_name,'/');
	if (!p)
		base_name = smb_fname->base_name;
	else
		base_name = p+1;

	/* NT expects the name to be in an exact form of the *full*
	   filename. See the trans2 torture test */
	if (ISDOT(base_name)) {
		dos_fname = talloc_strdup(mem_ctx, "\\");
		if (!dos_fname) {
			return NT_STATUS_NO_MEMORY;
		}
	} else {
		dos_fname = talloc_asprintf(mem_ctx,
				"\\%s",
				smb_fname->base_name);
		if (!dos_fname) {
			return NT_STATUS_NO_MEMORY;
		}
		if (is_ntfs_stream_smb_fname(smb_fname)) {
			dos_fname = talloc_asprintf(dos_fname, "%s",
						    smb_fname->stream_name);
			if (!dos_fname) {
				return NT_STATUS_NO_MEMORY;
			}
		}

		string_replace(dos_fname, '/', '\\');
	}

	allocation_size = SMB_VFS_GET_ALLOC_SIZE(conn, fsp, psbuf);

	if (fsp == NULL || !fsp->fsp_flags.is_fsa) {
		/* Do we have this path open ? */
		files_struct *fsp1;
		struct file_id fileid = vfs_file_id_from_sbuf(conn, psbuf);
		fsp1 = file_find_di_first(conn->sconn, fileid, true);
		if (fsp1 && fsp1->initial_allocation_size) {
			allocation_size = SMB_VFS_GET_ALLOC_SIZE(conn, fsp1, psbuf);
		}
	}

	if (!(mode & FILE_ATTRIBUTE_DIRECTORY)) {
		file_size = get_file_size_stat(psbuf);
	}

	if (fsp) {
		pos = fh_get_position_information(fsp->fh);
	}

	if (fsp) {
		access_mask = fsp->access_mask;
	} else {
		/* GENERIC_EXECUTE mapping from Windows */
		access_mask = 0x12019F;
	}

	/* This should be an index number - looks like
	   dev/ino to me :-)

	   I think this causes us to fail the IFSKIT
	   BasicFileInformationTest. -tpot */
	file_id = SMB_VFS_FS_FILE_ID(conn, psbuf);

	*fixed_portion = 0;

	switch (info_level) {
		case SMB_INFO_STANDARD:
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_INFO_STANDARD\n"));
			data_size = 22;
			srv_put_dos_date2(pdata,l1_fdateCreation,create_time);
			srv_put_dos_date2(pdata,l1_fdateLastAccess,atime);
			srv_put_dos_date2(pdata,l1_fdateLastWrite,mtime); /* write time */
			SIVAL(pdata,l1_cbFile,(uint32_t)file_size);
			SIVAL(pdata,l1_cbFileAlloc,(uint32_t)allocation_size);
			SSVAL(pdata,l1_attrFile,mode);
			break;

		case SMB_INFO_QUERY_EA_SIZE:
		{
			unsigned int ea_size =
			    estimate_ea_size(smb_fname->fsp);
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_INFO_QUERY_EA_SIZE\n"));
			data_size = 26;
			srv_put_dos_date2(pdata,0,create_time);
			srv_put_dos_date2(pdata,4,atime);
			srv_put_dos_date2(pdata,8,mtime); /* write time */
			SIVAL(pdata,12,(uint32_t)file_size);
			SIVAL(pdata,16,(uint32_t)allocation_size);
			SSVAL(pdata,20,mode);
			SIVAL(pdata,22,ea_size);
			break;
		}

		case SMB_INFO_IS_NAME_VALID:
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_INFO_IS_NAME_VALID\n"));
			if (fsp) {
				/* os/2 needs this ? really ?*/
				return NT_STATUS_DOS(ERRDOS, ERRbadfunc);
			}
			/* This is only reached for qpathinfo */
			data_size = 0;
			break;

		case SMB_INFO_QUERY_EAS_FROM_LIST:
		{
			size_t total_ea_len = 0;
			struct ea_list *ea_file_list = NULL;
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_INFO_QUERY_EAS_FROM_LIST\n"));

			status =
			    get_ea_list_from_fsp(mem_ctx,
						  smb_fname->fsp,
						  &total_ea_len, &ea_file_list);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}

			ea_list = ea_list_union(ea_list, ea_file_list, &total_ea_len);

			if (!ea_list || (total_ea_len > data_size)) {
				data_size = 4;
				SIVAL(pdata,0,4);   /* EA List Length must be set to 4 if no EA's. */
				break;
			}

			data_size = fill_ea_buffer(mem_ctx, pdata, data_size, conn, ea_list);
			break;
		}

		case SMB_INFO_QUERY_ALL_EAS:
		{
			/* We have data_size bytes to put EA's into. */
			size_t total_ea_len = 0;
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_INFO_QUERY_ALL_EAS\n"));

			status = get_ea_list_from_fsp(mem_ctx,
							smb_fname->fsp,
							&total_ea_len, &ea_list);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}

			if (!ea_list || (total_ea_len > data_size)) {
				data_size = 4;
				SIVAL(pdata,0,4);   /* EA List Length must be set to 4 if no EA's. */
				break;
			}

			data_size = fill_ea_buffer(mem_ctx, pdata, data_size, conn, ea_list);
			break;
		}

		case SMB2_FILE_FULL_EA_INFORMATION:
		{
			/* We have data_size bytes to put EA's into. */
			size_t total_ea_len = 0;
			struct ea_list *ea_file_list = NULL;

			DEBUG(10,("smbd_do_qfilepathinfo: SMB2_INFO_QUERY_ALL_EAS\n"));

			/*TODO: add filtering and index handling */

			status  =
				get_ea_list_from_fsp(mem_ctx,
						  smb_fname->fsp,
						  &total_ea_len, &ea_file_list);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}
			if (!ea_file_list) {
				return NT_STATUS_NO_EAS_ON_FILE;
			}

			status = fill_ea_chained_buffer(mem_ctx,
							pdata,
							data_size,
							&data_size,
							conn, ea_file_list);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}
			break;
		}

		case SMB_FILE_BASIC_INFORMATION:
		case SMB_QUERY_FILE_BASIC_INFO:

			if (info_level == SMB_QUERY_FILE_BASIC_INFO) {
				DEBUG(10,("smbd_do_qfilepathinfo: SMB_QUERY_FILE_BASIC_INFO\n"));
				data_size = 36; /* w95 returns 40 bytes not 36 - why ?. */
			} else {
				DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_BASIC_INFORMATION\n"));
				data_size = 40;
				SIVAL(pdata,36,0);
			}
			put_long_date_full_timespec(conn->ts_res,pdata,&create_time_ts);
			put_long_date_full_timespec(conn->ts_res,pdata+8,&atime_ts);
			put_long_date_full_timespec(conn->ts_res,pdata+16,&mtime_ts); /* write time */
			put_long_date_full_timespec(conn->ts_res,pdata+24,&ctime_ts); /* change time */
			SIVAL(pdata,32,mode);

			DEBUG(5,("SMB_QFBI - "));
			DEBUG(5,("create: %s ", ctime(&create_time)));
			DEBUG(5,("access: %s ", ctime(&atime)));
			DEBUG(5,("write: %s ", ctime(&mtime)));
			DEBUG(5,("change: %s ", ctime(&c_time)));
			DEBUG(5,("mode: %x\n", mode));
			*fixed_portion = data_size;
			break;

		case SMB_FILE_STANDARD_INFORMATION:
		case SMB_QUERY_FILE_STANDARD_INFO:

			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_STANDARD_INFORMATION\n"));
			data_size = 24;
			SOFF_T(pdata,0,allocation_size);
			SOFF_T(pdata,8,file_size);
			SIVAL(pdata,16,nlink);
			SCVAL(pdata,20,delete_pending?1:0);
			SCVAL(pdata,21,(mode&FILE_ATTRIBUTE_DIRECTORY)?1:0);
			SSVAL(pdata,22,0); /* Padding. */
			*fixed_portion = 24;
			break;

		case SMB_FILE_EA_INFORMATION:
		case SMB_QUERY_FILE_EA_INFO:
		{
			unsigned int ea_size =
			    estimate_ea_size(smb_fname->fsp);
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_EA_INFORMATION\n"));
			data_size = 4;
			*fixed_portion = 4;
			SIVAL(pdata,0,ea_size);
			break;
		}

		/* Get the 8.3 name - used if NT SMB was negotiated. */
		case SMB_QUERY_FILE_ALT_NAME_INFO:
		case SMB_FILE_ALTERNATE_NAME_INFORMATION:
		{
			char mangled_name[13];
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_ALTERNATE_NAME_INFORMATION\n"));
			if (!name_to_8_3(base_name,mangled_name,
						True,conn->params)) {
				return NT_STATUS_NO_MEMORY;
			}
			status = srvstr_push(dstart, flags2,
					  pdata+4, mangled_name,
					  PTR_DIFF(dend, pdata+4),
					  STR_UNICODE, &len);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}
			data_size = 4 + len;
			SIVAL(pdata,0,len);
			*fixed_portion = 8;
			break;
		}

		case SMB_QUERY_FILE_NAME_INFO:
		{
			/*
			  this must be *exactly* right for ACLs on mapped drives to work
			 */
			status = srvstr_push(dstart, flags2,
					  pdata+4, dos_fname,
					  PTR_DIFF(dend, pdata+4),
					  STR_UNICODE, &len);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_QUERY_FILE_NAME_INFO\n"));
			data_size = 4 + len;
			SIVAL(pdata,0,len);
			break;
		}

		case SMB_FILE_NORMALIZED_NAME_INFORMATION:
		{
			char *nfname = NULL;

			if (fsp == NULL || !fsp->conn->sconn->using_smb2) {
				return NT_STATUS_INVALID_LEVEL;
			}

			nfname = talloc_strdup(mem_ctx, smb_fname->base_name);
			if (nfname == NULL) {
				return NT_STATUS_NO_MEMORY;
			}

			if (ISDOT(nfname)) {
				nfname[0] = '\0';
			}
			string_replace(nfname, '/', '\\');

			if (smb_fname->stream_name != NULL) {
				const char *s = smb_fname->stream_name;
				const char *e = NULL;
				size_t n;

				SMB_ASSERT(s[0] != '\0');

				/*
				 * smb_fname->stream_name is in form
				 * of ':StrEam:$DATA', but we should only
				 * append ':StrEam' here.
				 */

				e = strchr(&s[1], ':');
				if (e == NULL) {
					n = strlen(s);
				} else {
					n = PTR_DIFF(e, s);
				}
				nfname = talloc_strndup_append(nfname, s, n);
				if (nfname == NULL) {
					return NT_STATUS_NO_MEMORY;
				}
			}

			status = srvstr_push(dstart, flags2,
					  pdata+4, nfname,
					  PTR_DIFF(dend, pdata+4),
					  STR_UNICODE, &len);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_NORMALIZED_NAME_INFORMATION\n"));
			data_size = 4 + len;
			SIVAL(pdata,0,len);
			*fixed_portion = 8;
			break;
		}

		case SMB_FILE_ALLOCATION_INFORMATION:
		case SMB_QUERY_FILE_ALLOCATION_INFO:
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_ALLOCATION_INFORMATION\n"));
			data_size = 8;
			SOFF_T(pdata,0,allocation_size);
			break;

		case SMB_FILE_END_OF_FILE_INFORMATION:
		case SMB_QUERY_FILE_END_OF_FILEINFO:
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_END_OF_FILE_INFORMATION\n"));
			data_size = 8;
			SOFF_T(pdata,0,file_size);
			break;

		case SMB_QUERY_FILE_ALL_INFO:
		case SMB_FILE_ALL_INFORMATION:
		{
			unsigned int ea_size =
			    estimate_ea_size(smb_fname->fsp);
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_ALL_INFORMATION\n"));
			put_long_date_full_timespec(conn->ts_res,pdata,&create_time_ts);
			put_long_date_full_timespec(conn->ts_res,pdata+8,&atime_ts);
			put_long_date_full_timespec(conn->ts_res,pdata+16,&mtime_ts); /* write time */
			put_long_date_full_timespec(conn->ts_res,pdata+24,&ctime_ts); /* change time */
			SIVAL(pdata,32,mode);
			SIVAL(pdata,36,0); /* padding. */
			pdata += 40;
			SOFF_T(pdata,0,allocation_size);
			SOFF_T(pdata,8,file_size);
			SIVAL(pdata,16,nlink);
			SCVAL(pdata,20,delete_pending);
			SCVAL(pdata,21,(mode&FILE_ATTRIBUTE_DIRECTORY)?1:0);
			SSVAL(pdata,22,0);
			pdata += 24;
			SIVAL(pdata,0,ea_size);
			pdata += 4; /* EA info */
			status = srvstr_push(dstart, flags2,
					  pdata+4, dos_fname,
					  PTR_DIFF(dend, pdata+4),
					  STR_UNICODE, &len);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}
			SIVAL(pdata,0,len);
			pdata += 4 + len;
			data_size = PTR_DIFF(pdata,(*ppdata));
			*fixed_portion = 10;
			break;
		}

		case SMB2_FILE_ALL_INFORMATION:
		{
			unsigned int ea_size =
			    estimate_ea_size(smb_fname->fsp);
			DEBUG(10,("smbd_do_qfilepathinfo: SMB2_FILE_ALL_INFORMATION\n"));
			put_long_date_full_timespec(conn->ts_res,pdata+0x00,&create_time_ts);
			put_long_date_full_timespec(conn->ts_res,pdata+0x08,&atime_ts);
			put_long_date_full_timespec(conn->ts_res,pdata+0x10,&mtime_ts); /* write time */
			put_long_date_full_timespec(conn->ts_res,pdata+0x18,&ctime_ts); /* change time */
			SIVAL(pdata,	0x20, mode);
			SIVAL(pdata,	0x24, 0); /* padding. */
			SBVAL(pdata,	0x28, allocation_size);
			SBVAL(pdata,	0x30, file_size);
			SIVAL(pdata,	0x38, nlink);
			SCVAL(pdata,	0x3C, delete_pending);
			SCVAL(pdata,	0x3D, (mode&FILE_ATTRIBUTE_DIRECTORY)?1:0);
			SSVAL(pdata,	0x3E, 0); /* padding */
			SBVAL(pdata,	0x40, file_id);
			SIVAL(pdata,	0x48, ea_size);
			SIVAL(pdata,	0x4C, access_mask);
			SBVAL(pdata,	0x50, pos);
			SIVAL(pdata,	0x58, mode); /*TODO: mode != mode fix this!!! */
			SIVAL(pdata,	0x5C, 0); /* No alignment needed. */

			pdata += 0x60;

			status = srvstr_push(dstart, flags2,
					  pdata+4, dos_fname,
					  PTR_DIFF(dend, pdata+4),
					  STR_UNICODE, &len);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}
			SIVAL(pdata,0,len);
			pdata += 4 + len;
			data_size = PTR_DIFF(pdata,(*ppdata));
			*fixed_portion = 104;
			break;
		}
		case SMB_FILE_INTERNAL_INFORMATION:

			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_INTERNAL_INFORMATION\n"));
			SBVAL(pdata, 0, file_id);
			data_size = 8;
			*fixed_portion = 8;
			break;

		case SMB_FILE_ACCESS_INFORMATION:
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_ACCESS_INFORMATION\n"));
			SIVAL(pdata, 0, access_mask);
			data_size = 4;
			*fixed_portion = 4;
			break;

		case SMB_FILE_NAME_INFORMATION:
			/* Pathname with leading '\'. */
			{
				size_t byte_len;
				byte_len = dos_PutUniCode(pdata+4,dos_fname,(size_t)max_data_bytes,False);
				DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_NAME_INFORMATION\n"));
				SIVAL(pdata,0,byte_len);
				data_size = 4 + byte_len;
				break;
			}

		case SMB_FILE_DISPOSITION_INFORMATION:
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_DISPOSITION_INFORMATION\n"));
			data_size = 1;
			SCVAL(pdata,0,delete_pending);
			*fixed_portion = 1;
			break;

		case SMB_FILE_POSITION_INFORMATION:
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_POSITION_INFORMATION\n"));
			data_size = 8;
			SOFF_T(pdata,0,pos);
			*fixed_portion = 8;
			break;

		case SMB_FILE_MODE_INFORMATION:
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_MODE_INFORMATION\n"));
			SIVAL(pdata,0,mode);
			data_size = 4;
			*fixed_portion = 4;
			break;

		case SMB_FILE_ALIGNMENT_INFORMATION:
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_ALIGNMENT_INFORMATION\n"));
			SIVAL(pdata,0,0); /* No alignment needed. */
			data_size = 4;
			*fixed_portion = 4;
			break;

		/*
		 * NT4 server just returns "invalid query" to this - if we try
		 * to answer it then NTws gets a BSOD! (tridge).  W2K seems to
		 * want this. JRA.
		 */
		/* The first statement above is false - verified using Thursby
		 * client against NT4 -- gcolley.
		 */
		case SMB_QUERY_FILE_STREAM_INFO:
		case SMB_FILE_STREAM_INFORMATION: {
			unsigned int num_streams = 0;
			struct stream_struct *streams = NULL;

			DEBUG(10,("smbd_do_qfilepathinfo: "
				  "SMB_FILE_STREAM_INFORMATION\n"));

			if (is_ntfs_stream_smb_fname(smb_fname)) {
				return NT_STATUS_INVALID_PARAMETER;
			}

			status = vfs_fstreaminfo(fsp,
						mem_ctx,
						&num_streams,
						&streams);

			if (!NT_STATUS_IS_OK(status)) {
				DEBUG(10, ("could not get stream info: %s\n",
					   nt_errstr(status)));
				return status;
			}

			status = marshall_stream_info(num_streams, streams,
						      pdata, max_data_bytes,
						      &data_size);

			if (!NT_STATUS_IS_OK(status)) {
				DEBUG(10, ("marshall_stream_info failed: %s\n",
					   nt_errstr(status)));
				TALLOC_FREE(streams);
				return status;
			}

			TALLOC_FREE(streams);

			*fixed_portion = 32;

			break;
		}
		case SMB_QUERY_COMPRESSION_INFO:
		case SMB_FILE_COMPRESSION_INFORMATION:
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_COMPRESSION_INFORMATION\n"));
			SOFF_T(pdata,0,file_size);
			SIVAL(pdata,8,0); /* ??? */
			SIVAL(pdata,12,0); /* ??? */
			data_size = 16;
			*fixed_portion = 16;
			break;

		case SMB_FILE_NETWORK_OPEN_INFORMATION:
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_NETWORK_OPEN_INFORMATION\n"));
			put_long_date_full_timespec(conn->ts_res,pdata,&create_time_ts);
			put_long_date_full_timespec(conn->ts_res,pdata+8,&atime_ts);
			put_long_date_full_timespec(conn->ts_res,pdata+16,&mtime_ts); /* write time */
			put_long_date_full_timespec(conn->ts_res,pdata+24,&ctime_ts); /* change time */
			SOFF_T(pdata,32,allocation_size);
			SOFF_T(pdata,40,file_size);
			SIVAL(pdata,48,mode);
			SIVAL(pdata,52,0); /* ??? */
			data_size = 56;
			*fixed_portion = 56;
			break;

		case SMB_FILE_ATTRIBUTE_TAG_INFORMATION:
			DEBUG(10,("smbd_do_qfilepathinfo: SMB_FILE_ATTRIBUTE_TAG_INFORMATION\n"));
			SIVAL(pdata,0,mode);
			SIVAL(pdata,4,0);
			data_size = 8;
			*fixed_portion = 8;
			break;

		/*
		 * CIFS UNIX Extensions.
		 */

		case SMB_QUERY_FILE_UNIX_BASIC:

			pdata = store_file_unix_basic(conn, pdata, fsp, psbuf);
			data_size = PTR_DIFF(pdata,(*ppdata));

			DEBUG(4,("smbd_do_qfilepathinfo: "
				 "SMB_QUERY_FILE_UNIX_BASIC\n"));
			dump_data(4, (uint8_t *)(*ppdata), data_size);

			break;

		case SMB_QUERY_FILE_UNIX_INFO2:

			pdata = store_file_unix_basic_info2(conn, pdata, fsp, psbuf);
			data_size = PTR_DIFF(pdata,(*ppdata));

			{
				int i;
				DEBUG(4,("smbd_do_qfilepathinfo: SMB_QUERY_FILE_UNIX_INFO2 "));

				for (i=0; i<100; i++)
					DEBUG(4,("%d=%x, ",i, (*ppdata)[i]));
				DEBUG(4,("\n"));
			}

			break;

		case SMB_QUERY_FILE_UNIX_LINK:
			{
				status = smb_unix_read_symlink(conn,
							req,
							smb_fname,
							pdata,
							data_size,
							&data_size);
				if (!NT_STATUS_IS_OK(status)) {
					return status;
				}
				break;
			}

#if defined(HAVE_POSIX_ACLS)
		case SMB_QUERY_POSIX_ACL:
			{
				status = smb_query_posix_acl(conn,
							req,
							fsp,
							smb_fname,
							pdata,
							data_size,
							&data_size);
				if (!NT_STATUS_IS_OK(status)) {
					return status;
				}
				break;
			}
#endif


		case SMB_QUERY_POSIX_LOCK:
		{
			uint64_t count;
			uint64_t offset;
			uint64_t smblctx;
			enum brl_type lock_type;

			/* We need an open file with a real fd for this. */
			if (fsp == NULL ||
			    fsp->fsp_flags.is_pathref ||
			    fsp_get_io_fd(fsp) == -1)
			{
				return NT_STATUS_INVALID_LEVEL;
			}

			if (lock_data_count != POSIX_LOCK_DATA_SIZE) {
				return NT_STATUS_INVALID_PARAMETER;
			}

			switch (SVAL(pdata, POSIX_LOCK_TYPE_OFFSET)) {
				case POSIX_LOCK_TYPE_READ:
					lock_type = READ_LOCK;
					break;
				case POSIX_LOCK_TYPE_WRITE:
					lock_type = WRITE_LOCK;
					break;
				case POSIX_LOCK_TYPE_UNLOCK:
				default:
					/* There's no point in asking for an unlock... */
					return NT_STATUS_INVALID_PARAMETER;
			}

			smblctx = (uint64_t)IVAL(pdata, POSIX_LOCK_PID_OFFSET);
			offset = BVAL(pdata,POSIX_LOCK_START_OFFSET);
			count = BVAL(pdata,POSIX_LOCK_LEN_OFFSET);

			status = query_lock(fsp,
					&smblctx,
					&count,
					&offset,
					&lock_type,
					POSIX_LOCK);

			if (ERROR_WAS_LOCK_DENIED(status)) {
				/* Here we need to report who has it locked... */
				data_size = POSIX_LOCK_DATA_SIZE;

				SSVAL(pdata, POSIX_LOCK_TYPE_OFFSET, lock_type);
				SSVAL(pdata, POSIX_LOCK_FLAGS_OFFSET, 0);
				SIVAL(pdata, POSIX_LOCK_PID_OFFSET, (uint32_t)smblctx);
				SBVAL(pdata, POSIX_LOCK_START_OFFSET, offset);
				SBVAL(pdata, POSIX_LOCK_LEN_OFFSET, count);

			} else if (NT_STATUS_IS_OK(status)) {
				/* For success we just return a copy of what we sent
				   with the lock type set to POSIX_LOCK_TYPE_UNLOCK. */
				data_size = POSIX_LOCK_DATA_SIZE;
				memcpy(pdata, lock_data, POSIX_LOCK_DATA_SIZE);
				SSVAL(pdata, POSIX_LOCK_TYPE_OFFSET, POSIX_LOCK_TYPE_UNLOCK);
			} else {
				return status;
			}
			break;
		}

		default:
			return NT_STATUS_INVALID_LEVEL;
	}

	*pdata_size = data_size;
	return NT_STATUS_OK;
}