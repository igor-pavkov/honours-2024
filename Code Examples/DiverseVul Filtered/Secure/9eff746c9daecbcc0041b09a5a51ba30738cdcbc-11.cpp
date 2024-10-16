int sql_auxprop_plug_init(const sasl_utils_t *utils,
			  int max_version,
			  int *out_version,
			  sasl_auxprop_plug_t **plug,
			  const char *plugname __attribute__((unused))) 
{
    sql_settings_t *settings;
    
    if (!out_version || !plug) return SASL_BADPARAM;
    
    if (max_version < SASL_AUXPROP_PLUG_VERSION) return SASL_BADVERS;
    *out_version = SASL_AUXPROP_PLUG_VERSION;
    
    *plug = &sql_auxprop_plugin;
    
    settings = (sql_settings_t *) utils->malloc(sizeof(sql_settings_t));
    
    if (!settings) {
	MEMERROR(utils);
	return SASL_NOMEM;
    }
    
    memset(settings, 0, sizeof(sql_settings_t));
    sql_get_settings(utils, settings);
    
    if (!settings->sql_engine->name) return SASL_NOMECH;

    if (!sql_exists(settings->sql_select)) {
	utils->log(utils->conn, SASL_LOG_ERR, "sql_select option missing");
	utils->free(settings);	
	return SASL_NOMECH;
    }

    utils->log(utils->conn, SASL_LOG_DEBUG,
	       "sql auxprop plugin using %s engine\n",
	       settings->sql_engine->name);
    
    sql_auxprop_plugin.glob_context = settings;
    
    return SASL_OK;
}