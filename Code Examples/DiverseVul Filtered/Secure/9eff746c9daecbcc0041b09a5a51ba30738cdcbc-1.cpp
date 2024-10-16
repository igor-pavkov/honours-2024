static void sql_auxprop_free(void *glob_context, const sasl_utils_t *utils)
{
    sql_settings_t *settings;
    
    settings = (sql_settings_t *)glob_context;
    
    if (!settings) return;
    
    utils->log(utils->conn, SASL_LOG_DEBUG, "sql freeing memory\n");
    
    utils->free(settings);
}