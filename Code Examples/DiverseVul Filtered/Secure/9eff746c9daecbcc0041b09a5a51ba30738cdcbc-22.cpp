static void *_mysql_open(char *host, char *port, int usessl,
			 const char *user, const char *password,
			 const char *database, const sasl_utils_t *utils)
{
    MYSQL *mysql;
    
    if (!(mysql = mysql_init(NULL))) {
	utils->log(utils->conn, SASL_LOG_ERR,
		   "sql plugin: could not execute mysql_init()");
	return NULL;
    }
    
    return mysql_real_connect(mysql, host, user, password, database,
			      port ? strtoul(port, NULL, 10) : 0, NULL,
			      usessl ? CLIENT_SSL : 0);
}