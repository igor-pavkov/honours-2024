static char *store_file_unix_basic_info2(connection_struct *conn,
				char *pdata,
				files_struct *fsp,
				const SMB_STRUCT_STAT *psbuf)
{
	uint32_t file_flags = 0;
	uint32_t flags_mask = 0;

	pdata = store_file_unix_basic(conn, pdata, fsp, psbuf);

	/* Create (birth) time 64 bit */
	put_long_date_full_timespec(TIMESTAMP_SET_NT_OR_BETTER,pdata, &psbuf->st_ex_btime);
	pdata += 8;

	map_info2_flags_from_sbuf(psbuf, &file_flags, &flags_mask);
	SIVAL(pdata, 0, file_flags); /* flags */
	SIVAL(pdata, 4, flags_mask); /* mask */
	pdata += 8;

	return pdata;
}