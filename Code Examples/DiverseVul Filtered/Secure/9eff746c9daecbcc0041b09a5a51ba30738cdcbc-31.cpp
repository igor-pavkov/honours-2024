static int _pgsql_exec(void *conn, const char *cmd, char *value, size_t size,
		       size_t *value_len, const sasl_utils_t *utils)
{
    PGresult *result;
    int row_count;
    ExecStatusType status;
    
    /* run the query */
    result = PQexec(conn, cmd);
    
    /* check the status */
    status = PQresultStatus(result);
    if (status == PGRES_COMMAND_OK) {
	/* no results (BEGIN, COMMIT, DELETE, INSERT, UPDATE) */
	PQclear(result);
	return 0;
    }
    else if (status != PGRES_TUPLES_OK) {
	/* error */
	utils->log(utils->conn, SASL_LOG_DEBUG, "sql plugin: %s ",
		   PQresStatus(status));
	PQclear(result);
	return -1;
    }
    
    /* quick row check */
    row_count = PQntuples(result);
    if (!row_count) {
	/* umm nothing found */
	utils->log(utils->conn, SASL_LOG_NOTE, "sql plugin: no result found");
	PQclear(result);
	return -1;
    }
    if (row_count > 1) {
	utils->log(utils->conn, SASL_LOG_WARN,
		   "sql plugin: found duplicate row for query %s", cmd);
    }
    
    /* now get the result set value and value_len */
    /* we only fetch one because we don't care about the rest */
    if (value) {
	strncpy(value, PQgetvalue(result,0,0), size-2);
	value[size-1] = '\0';
	if (value_len) *value_len = strlen(value);
    }
    
    /* free result */
    PQclear(result);
    return 0;
}