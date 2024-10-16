static void *_sqlite_open(char *host __attribute__((unused)),
			  char *port __attribute__((unused)),
			  int usessl __attribute__((unused)),
			  const char *user __attribute__((unused)),
			  const char *password __attribute__((unused)),
			  const char *database, const sasl_utils_t *utils)
{
    int rc;
    sqlite *db;
    char *zErrMsg = NULL;

    db = sqlite_open(database, 0, &zErrMsg);
    if (db == NULL) {
	utils->log(utils->conn, SASL_LOG_ERR, "sql plugin: %s", zErrMsg);
	sqlite_freemem (zErrMsg);
	return NULL;
    }

    rc = sqlite_exec(db, "PRAGMA empty_result_callbacks = ON", NULL, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
	utils->log(utils->conn, SASL_LOG_ERR, "sql plugin: %s", zErrMsg);
	sqlite_freemem (zErrMsg);
	sqlite_close(db);
	return NULL;
    }

    return (void*)db;
}