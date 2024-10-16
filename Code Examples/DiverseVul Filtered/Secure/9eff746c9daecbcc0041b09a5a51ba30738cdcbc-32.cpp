static void *_sqlite3_open(char *host __attribute__((unused)),
			  char *port __attribute__((unused)),
			  int usessl __attribute__((unused)),
			  const char *user __attribute__((unused)),
			  const char *password __attribute__((unused)),
			  const char *database, const sasl_utils_t *utils)
{
    int rc;
    sqlite3 *db;
    char *zErrMsg = NULL;

    rc = sqlite3_open(database, &db);
    if (SQLITE_OK != rc) {
    	if (db)
		utils->log(utils->conn, SASL_LOG_ERR, "sql plugin: %s",
			   sqlite3_errmsg(db));
	else
		utils->log(utils->conn, SASL_LOG_ERR, "sql plugin: %d", rc);
	sqlite3_close(db);
	return NULL;
    }

    rc = sqlite3_exec(db, "PRAGMA empty_result_callbacks = ON", NULL, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
    	if (zErrMsg) {
		utils->log(utils->conn, SASL_LOG_ERR, "sql plugin: %s",
			   zErrMsg);
		sqlite3_free(zErrMsg);
	} else
		utils->log(utils->conn, SASL_LOG_DEBUG, "sql plugin: %d", rc);
	sqlite3_close(db);
	return NULL;
    }

    return (void*)db;
}