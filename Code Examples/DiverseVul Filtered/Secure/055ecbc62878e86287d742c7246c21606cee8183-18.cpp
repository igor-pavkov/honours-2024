int _pdo_sqlite_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *file, int line TSRMLS_DC) /* {{{ */
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	pdo_error_type *pdo_err = stmt ? &stmt->error_code : &dbh->error_code;
	pdo_sqlite_error_info *einfo = &H->einfo;

	einfo->errcode = sqlite3_errcode(H->db);
	einfo->file = file;
	einfo->line = line;

	if (einfo->errcode != SQLITE_OK) {
		if (einfo->errmsg) {
			pefree(einfo->errmsg, dbh->is_persistent);
		}
		einfo->errmsg = pestrdup((char*)sqlite3_errmsg(H->db), dbh->is_persistent);
	} else { /* no error */
		strncpy(*pdo_err, PDO_ERR_NONE, sizeof(PDO_ERR_NONE));
		return 0;
	}
	switch (einfo->errcode) {
		case SQLITE_NOTFOUND:
			strncpy(*pdo_err, "42S02", sizeof("42S02"));
			break;	

		case SQLITE_INTERRUPT:
			strncpy(*pdo_err, "01002", sizeof("01002"));
			break;

		case SQLITE_NOLFS:
			strncpy(*pdo_err, "HYC00", sizeof("HYC00"));
			break;

		case SQLITE_TOOBIG:
			strncpy(*pdo_err, "22001", sizeof("22001"));
			break;
	
		case SQLITE_CONSTRAINT:
			strncpy(*pdo_err, "23000", sizeof("23000"));
			break;

		case SQLITE_ERROR:
		default:
			strncpy(*pdo_err, "HY000", sizeof("HY000"));
			break;
	}

	if (!dbh->methods) {
		zend_throw_exception_ex(php_pdo_get_exception(), einfo->errcode TSRMLS_CC, "SQLSTATE[%s] [%d] %s",
				*pdo_err, einfo->errcode, einfo->errmsg);
	}
	
	return einfo->errcode;
}