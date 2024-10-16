void ha_prepare_for_backup()
{
  plugin_foreach_with_mask(0, plugin_prepare_for_backup,
                           MYSQL_STORAGE_ENGINE_PLUGIN,
                           PLUGIN_IS_DELETED|PLUGIN_IS_READY, 0);
}