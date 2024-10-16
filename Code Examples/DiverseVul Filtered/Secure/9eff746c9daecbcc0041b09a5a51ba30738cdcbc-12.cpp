static int _mysql_exec(void *conn, const char *cmd, char *value, size_t size,
		       size_t *value_len, const sasl_utils_t *utils)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    int row_count, len;
    
    len = strlen(cmd);
    /* mysql_real_query() doesn't want a terminating ';' */
    if (cmd[len-1] == ';') len--;

    /* 
     *  Run the query. It is important to note that mysql_real_query
     *  will return success even if the sql statement 
     *  had an error in it. However, mysql_errno() will alsways
     *  tell us if there was an error. Therefore we can ignore
     *  the result from mysql_real_query and simply check mysql_errno()
     *  to decide if there was really an error.
     */
    (void)mysql_real_query(conn, cmd, len);

    if(mysql_errno(conn)) {
        utils->log(utils->conn, SASL_LOG_ERR, "sql query failed: %s",
		   mysql_error(conn));
	return -1;
    }

    /* see if we should expect some results */
    if (!mysql_field_count(conn)) {
	/* no results (BEGIN, COMMIT, DELETE, INSERT, UPDATE) */
	return 0;
    }

    /* get the results */
    result = mysql_store_result(conn);
    if (!result) {
	/* umm nothing found */
	utils->log(utils->conn, SASL_LOG_NOTE, "sql plugin: no result found");
	return -1;
    }

    /* quick row check */
    row_count = mysql_num_rows(result);
    if (!row_count) {
	/* umm nothing found */
	mysql_free_result(result);
	utils->log(utils->conn, SASL_LOG_NOTE, "sql plugin: no result found");
	return -1;
    }
    if (row_count > 1) {
	utils->log(utils->conn, SASL_LOG_WARN,
		   "sql plugin: found duplicate row for query %s", cmd);
    }
    
    /* now get the result set value and value_len */
    /* we only fetch one because we don't care about the rest */
    row = mysql_fetch_row(result);
    if (!row || !row[0]) {
	/* umm nothing found */
	utils->log(utils->conn, SASL_LOG_NOTE, "sql plugin: no result found");
	mysql_free_result(result);
	return -1;
    }
    if (value) {
	strncpy(value, row[0], size-2);
	value[size-1] = '\0';
	if (value_len) *value_len = strlen(value);
    }
    
    /* free result */
    mysql_free_result(result);
    mysql_next_result(conn);
    
    return 0;
}