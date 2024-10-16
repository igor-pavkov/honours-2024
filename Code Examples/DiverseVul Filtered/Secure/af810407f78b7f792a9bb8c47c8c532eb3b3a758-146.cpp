static plugin_ref ha_default_plugin(THD *thd)
{
  if (thd->variables.table_plugin)
    return thd->variables.table_plugin;
  return my_plugin_lock(thd, global_system_variables.table_plugin);
}