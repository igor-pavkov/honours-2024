static plugin_ref ha_default_tmp_plugin(THD *thd)
{
  if (thd->variables.tmp_table_plugin)
    return thd->variables.tmp_table_plugin;
  if (global_system_variables.tmp_table_plugin)
    return my_plugin_lock(thd, global_system_variables.tmp_table_plugin);
  return ha_default_plugin(thd);
}