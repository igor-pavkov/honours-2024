int csync_unlink(const char *filename, int ign)
{
	struct stat st;
	int rc;

	if ( lstat_strict(prefixsubst(filename), &st) != 0 ) return 0;
	if ( ign==2 && S_ISREG(st.st_mode) ) return 0;
	rc = S_ISDIR(st.st_mode) ? rmdir(prefixsubst(filename)) : unlink(prefixsubst(filename));

	if ( rc && !ign ) cmd_error = strerror(errno);
	return rc;
}