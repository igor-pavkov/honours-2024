static char *store_file_unix_basic(connection_struct *conn,
				char *pdata,
				files_struct *fsp,
				const SMB_STRUCT_STAT *psbuf)
{
	dev_t devno;

	DEBUG(10,("store_file_unix_basic: SMB_QUERY_FILE_UNIX_BASIC\n"));
	DEBUG(4,("store_file_unix_basic: st_mode=%o\n",(int)psbuf->st_ex_mode));

	SOFF_T(pdata,0,get_file_size_stat(psbuf));             /* File size 64 Bit */
	pdata += 8;

	SOFF_T(pdata,0,SMB_VFS_GET_ALLOC_SIZE(conn,fsp,psbuf)); /* Number of bytes used on disk - 64 Bit */
	pdata += 8;

	put_long_date_full_timespec(TIMESTAMP_SET_NT_OR_BETTER, pdata, &psbuf->st_ex_ctime);       /* Change Time 64 Bit */
	put_long_date_full_timespec(TIMESTAMP_SET_NT_OR_BETTER ,pdata+8, &psbuf->st_ex_atime);     /* Last access time 64 Bit */
	put_long_date_full_timespec(TIMESTAMP_SET_NT_OR_BETTER, pdata+16, &psbuf->st_ex_mtime);    /* Last modification time 64 Bit */
	pdata += 24;

	SIVAL(pdata,0,psbuf->st_ex_uid);               /* user id for the owner */
	SIVAL(pdata,4,0);
	pdata += 8;

	SIVAL(pdata,0,psbuf->st_ex_gid);               /* group id of owner */
	SIVAL(pdata,4,0);
	pdata += 8;

	SIVAL(pdata,0,unix_filetype(psbuf->st_ex_mode));
	pdata += 4;

	if (S_ISBLK(psbuf->st_ex_mode) || S_ISCHR(psbuf->st_ex_mode)) {
		devno = psbuf->st_ex_rdev;
	} else {
		devno = psbuf->st_ex_dev;
	}

	SIVAL(pdata,0,unix_dev_major(devno));   /* Major device number if type is device */
	SIVAL(pdata,4,0);
	pdata += 8;

	SIVAL(pdata,0,unix_dev_minor(devno));   /* Minor device number if type is device */
	SIVAL(pdata,4,0);
	pdata += 8;

	SINO_T_VAL(pdata, 0, psbuf->st_ex_ino);   /* inode number */
	pdata += 8;

	SIVAL(pdata,0, unix_perms_to_wire(psbuf->st_ex_mode));     /* Standard UNIX file permissions */
	SIVAL(pdata,4,0);
	pdata += 8;

	SIVAL(pdata,0,psbuf->st_ex_nlink);             /* number of hard links */
	SIVAL(pdata,4,0);
	pdata += 8;

	return pdata;
}