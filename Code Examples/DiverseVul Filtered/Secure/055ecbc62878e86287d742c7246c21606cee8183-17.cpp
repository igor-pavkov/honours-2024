static int sqlite_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, zval *driver_options TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	pdo_sqlite_stmt *S = ecalloc(1, sizeof(pdo_sqlite_stmt));
	int i;
	const char *tail;

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &sqlite_stmt_methods;
	stmt->supports_placeholders = PDO_PLACEHOLDER_POSITIONAL|PDO_PLACEHOLDER_NAMED;

	if (PDO_CURSOR_FWDONLY != pdo_attr_lval(driver_options, PDO_ATTR_CURSOR, PDO_CURSOR_FWDONLY TSRMLS_CC)) {
		H->einfo.errcode = SQLITE_ERROR;
		pdo_sqlite_error(dbh);
		return 0;
	}

	i = sqlite3_prepare(H->db, sql, sql_len, &S->stmt, &tail);
	if (i == SQLITE_OK) {
		return 1;
	}

	pdo_sqlite_error(dbh);

	return 0;
}