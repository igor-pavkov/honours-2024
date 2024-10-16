void ha_disable_internal_writes(bool disable)
{
  plugin_foreach(NULL, plugin_disable_internal_writes,
                 MYSQL_STORAGE_ENGINE_PLUGIN, &disable);
}