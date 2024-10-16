static int sqlite_handle_closer(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	
	if (H) {
		pdo_sqlite_error_info *einfo = &H->einfo;

		pdo_sqlite_cleanup_callbacks(H TSRMLS_CC);
		if (H->db) {
			sqlite3_close(H->db);
			H->db = NULL;
		}
		if (einfo->errmsg) {
			pefree(einfo->errmsg, dbh->is_persistent);
			einfo->errmsg = NULL;
		}
		pefree(H, dbh->is_persistent);
		dbh->driver_data = NULL;
	}
	return 0;
}