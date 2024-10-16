int csync_check_dirty(const char *filename, const char *peername, int isflush)
{
	int rc = 0;
	csync_check(filename, 0, 0);
	if (isflush) return 0;
	SQL_BEGIN("Check if file is dirty",
		"SELECT 1 FROM dirty WHERE filename = '%s' LIMIT 1",
		url_encode(filename))
	{
		rc = 1;
		cmd_error = conn_response(CR_ERR_ALSO_DIRTY_HERE);
	} SQL_END;
	if (rc && peername)
		csync_mark(filename, peername, 0);
	return rc;
}