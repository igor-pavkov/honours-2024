static void *_pgsql_open(char *host, char *port, int usessl,
			 const char *user, const char *password,
			 const char *database, const sasl_utils_t *utils)
{
    PGconn *conn = NULL;
    char *conninfo, *sep;
    
    /* create the connection info string */
    /* The 64 represents the number of characters taken by
     * the keyword tokens, plus a small pad
     */
    conninfo = utils->malloc(64 + sql_len(host) + sql_len(port)
			     + sql_len(user) + sql_len(password)
			     + sql_len(database));
    if (!conninfo) {
	MEMERROR(utils);
	return NULL;
    }
    
    /* add each term that exists */
    conninfo[0] = '\0';
    sep = "";
    if (sql_exists(host)) {
	strcat(conninfo, sep);
	strcat(conninfo, "host='");
	strcat(conninfo, host);
	strcat(conninfo, "'");
	sep = " ";
    }
    if (sql_exists(port)) {
	strcat(conninfo, sep);
	strcat(conninfo, "port='");
	strcat(conninfo, port);
	strcat(conninfo, "'");
	sep = " ";
    }
    if (sql_exists(user)) {
	strcat(conninfo, sep);
	strcat(conninfo, "user='");
	strcat(conninfo, user);
	strcat(conninfo, "'");
	sep = " ";
    }
    if (sql_exists(password)) {
	strcat(conninfo, sep);
	strcat(conninfo, "password='");
	strcat(conninfo, password);
	strcat(conninfo, "'");
	sep = " ";
    }
    if (sql_exists(database)) {
	strcat(conninfo, sep);
	strcat(conninfo, "dbname='");
	strcat(conninfo, database);
	strcat(conninfo, "'");
	sep = " ";
    }
    if (usessl) {
	strcat(conninfo, sep);
	strcat(conninfo, "requiressl='1'");
    }
    
    conn = PQconnectdb(conninfo);
    free(conninfo);
    
    if ((PQstatus(conn) != CONNECTION_OK)) {
	utils->log(utils->conn, SASL_LOG_ERR, "sql plugin: %s",
		   PQerrorMessage(conn));
	return NULL;
    }
    
    return conn;
}