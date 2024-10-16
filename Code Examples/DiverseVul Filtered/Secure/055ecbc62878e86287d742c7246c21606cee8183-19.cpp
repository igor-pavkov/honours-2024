static int pdo_sqlite_set_attr(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;

	switch (attr) {
		case PDO_ATTR_TIMEOUT:
			convert_to_long(val);
			sqlite3_busy_timeout(H->db, Z_LVAL_P(val) * 1000);
			return 1;
	}
	return 0;
}