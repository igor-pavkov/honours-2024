void ha_kill_query(THD* thd, enum thd_kill_levels level)
{
  DBUG_ENTER("ha_kill_query");
  plugin_foreach(thd, kill_handlerton, MYSQL_STORAGE_ENGINE_PLUGIN, &level);
  DBUG_VOID_RETURN;
}