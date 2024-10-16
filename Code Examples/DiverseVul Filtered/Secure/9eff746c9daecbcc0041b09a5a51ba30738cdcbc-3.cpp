static void *sql_connect(sql_settings_t *settings, const sasl_utils_t *utils)
{
    void *conn = NULL;
    char *db_host_ptr = NULL;
    char *db_host = NULL;
    char *cur_host, *cur_port;

    /* loop around hostnames till we get a connection 
     * it should probably save the connection but for 
     * now we will just disconnect everytime
     */
    utils->log(utils->conn, SASL_LOG_DEBUG,
	       "sql plugin try and connect to a host\n");
    
    /* create a working version of the hostnames */
    _plug_strdup(utils, settings->sql_hostnames, &db_host_ptr, NULL);
    db_host = db_host_ptr;
    cur_host = db_host;
    while (cur_host != NULL) {
	db_host = strchr(db_host,',');
	if (db_host != NULL) {  
	    db_host[0] = '\0';

	    /* loop till we find some text */
	    while (!isalnum(db_host[0])) db_host++;
	}
	
	utils->log(utils->conn, SASL_LOG_DEBUG,
		   "sql plugin trying to open db '%s' on host '%s'%s\n",
		   settings->sql_database, cur_host,
		   settings->sql_usessl ? " using SSL" : "");
	
	/* set the optional port */
	if ((cur_port = strchr(cur_host, ':'))) *cur_port++ = '\0';
	
	conn = settings->sql_engine->sql_open(cur_host, cur_port,
					      settings->sql_usessl,
					      settings->sql_user,
					      settings->sql_passwd,
					      settings->sql_database,
					      utils);
	if (conn) break;
	
	utils->log(utils->conn, SASL_LOG_ERR,
		   "sql plugin could not connect to host %s", cur_host);
	
	cur_host = db_host;
    }

    if (db_host_ptr) utils->free(db_host_ptr);

    return conn;
}