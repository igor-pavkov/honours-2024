void ha_close_connection(THD* thd)
{
  plugin_foreach_with_mask(thd, closecon_handlerton,
                           MYSQL_STORAGE_ENGINE_PLUGIN,
                           PLUGIN_IS_DELETED|PLUGIN_IS_READY, 0);
}