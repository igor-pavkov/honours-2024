static int _sqlite3_exec(void *db,
			 const char *cmd,
			 char *value,
			 size_t size,
			 size_t *value_len,
			 const sasl_utils_t *utils)
{
    int rc;
    char *result = NULL;
    char *zErrMsg = NULL;

    rc = sqlite3_exec((sqlite3*)db, cmd, sqlite3_my_callback, (void*)&result, &zErrMsg);
    if (rc != SQLITE_OK) {
    	if (zErrMsg) {
	    utils->log(utils->conn, SASL_LOG_DEBUG, "sql plugin: %s", zErrMsg);
	    sqlite3_free(zErrMsg);
	} else {
	    utils->log(utils->conn, SASL_LOG_DEBUG, "sql plugin: %d", rc);
	}
	return -1;
    }

    if (value == NULL && rc == SQLITE_OK) {
	/* no results (BEGIN, COMMIT, DELETE, INSERT, UPDATE) */
	return 0;
    }

    if (result == NULL) {
	/* umm nothing found */
	utils->log(utils->conn, SASL_LOG_NOTE, "sql plugin: no result found");
	return -1;
    }

    /* XXX: Duplication cannot be found by this method. */

    /* now get the result set value and value_len */
    /* we only fetch one because we don't care about the rest */
    if (value) {
	strncpy(value, result, size - 2);
	value[size - 1] = '\0';
	if (value_len) {
	    *value_len = strlen(value);
	}
    }

    free(result);
    return 0;
}