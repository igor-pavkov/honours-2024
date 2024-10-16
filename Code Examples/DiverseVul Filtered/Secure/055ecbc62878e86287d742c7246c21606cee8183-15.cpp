static void pdo_sqlite_request_shutdown(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	/* unregister functions, so that they don't linger for the next
	 * request */
	if (H) {
		pdo_sqlite_cleanup_callbacks(H TSRMLS_CC);
	}
}