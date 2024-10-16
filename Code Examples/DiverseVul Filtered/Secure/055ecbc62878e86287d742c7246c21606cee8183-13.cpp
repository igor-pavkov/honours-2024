static int sqlite_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen, enum pdo_param_type paramtype  TSRMLS_DC)
{
	*quoted = safe_emalloc(2, unquotedlen, 3);
	sqlite3_snprintf(2*unquotedlen + 3, *quoted, "'%q'", unquoted);
	*quotedlen = strlen(*quoted);
	return 1;
}