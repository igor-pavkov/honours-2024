static void sql_get_settings(const sasl_utils_t *utils, void *glob_context)
{
    sql_settings_t *settings;
    int r;
    const char *usessl, *engine_name;
    const sql_engine_t *e;
    
    settings = (sql_settings_t *) glob_context;
    
    r = utils->getopt(utils->getopt_context,"SQL", "sql_engine",
		      &engine_name, NULL);
    if (r || !engine_name) {
	engine_name = "mysql";
    }
    
    /* find the correct engine */
    e = sql_engines;
    while (e->name) {
	if (!strcasecmp(engine_name, e->name)) break;
	e++;
    }

    if (!e->name) {
	utils->log(utils->conn, SASL_LOG_ERR, "SQL engine '%s' not supported",
		   engine_name);
    }

    settings->sql_engine = e;

    r = utils->getopt(utils->getopt_context,"SQL","sql_user",
		      &settings->sql_user, NULL);
    if ( r || !settings->sql_user ) {
	settings->sql_user = SQL_BLANK_STRING;
    }

    r = utils->getopt(utils->getopt_context,"SQL", "sql_passwd",
		      &settings->sql_passwd, NULL);
    if (r || !settings->sql_passwd ) {
	settings->sql_passwd = SQL_BLANK_STRING;
    }

    r = utils->getopt(utils->getopt_context,"SQL", "sql_hostnames",
		      &settings->sql_hostnames, NULL);
    if (r || !settings->sql_hostnames ) {
	settings->sql_hostnames = SQL_BLANK_STRING;
    }

    r = utils->getopt(utils->getopt_context,"SQL", "sql_database",
		      &settings->sql_database, NULL);
    if (r || !settings->sql_database ) {
	settings->sql_database = SQL_BLANK_STRING;
    }

    r = utils->getopt(utils->getopt_context,"SQL", "sql_select",
		      &settings->sql_select, NULL);
    if (r || !settings->sql_select ) {
	/* backwards compatibility */
	r = utils->getopt(utils->getopt_context,"SQL", "sql_statement",
			  &settings->sql_select, NULL);
	if (r || !settings->sql_select) {
	    settings->sql_select = SQL_BLANK_STRING;
	}
    }

    r = utils->getopt(utils->getopt_context, "SQL", "sql_insert",
		      &settings->sql_insert, NULL);
    if (r || !settings->sql_insert) {
	settings->sql_insert = SQL_BLANK_STRING;
    }

    r = utils->getopt(utils->getopt_context, "SQL", "sql_update",
		      &settings->sql_update, NULL);
    if (r || !settings->sql_update) {
	settings->sql_update = SQL_BLANK_STRING;
    }

    r = utils->getopt(utils->getopt_context, "SQL", "sql_usessl",
		  &usessl, NULL);
    if (r || !usessl) usessl = "no";

    if (*usessl == '1' || *usessl == 'y'  || *usessl == 't' ||
	(*usessl == 'o' && usessl[1] == 'n')) {
	settings->sql_usessl = 1;
    } else {
	settings->sql_usessl = 0;
    }
}