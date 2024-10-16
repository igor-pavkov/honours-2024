static int sqlite_handle_begin(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	char *errmsg = NULL;

	if (sqlite3_exec(H->db, "BEGIN", NULL, NULL, &errmsg) != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		if (errmsg)
			sqlite3_free(errmsg);
		return 0;
	}
	return 1;
}