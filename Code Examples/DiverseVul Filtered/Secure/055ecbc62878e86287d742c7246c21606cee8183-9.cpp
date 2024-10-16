static long sqlite_handle_doer(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	char *errmsg = NULL;

	if (sqlite3_exec(H->db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		if (errmsg)
			sqlite3_free(errmsg);

		return -1;
	} else {
		return sqlite3_changes(H->db);
	}
}