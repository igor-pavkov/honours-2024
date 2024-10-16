static char *pdo_sqlite_last_insert_id(pdo_dbh_t *dbh, const char *name, unsigned int *len TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	char *id;
	
	id = php_pdo_int64_to_str(sqlite3_last_insert_rowid(H->db) TSRMLS_CC);
	*len = strlen(id);
	return id;
}