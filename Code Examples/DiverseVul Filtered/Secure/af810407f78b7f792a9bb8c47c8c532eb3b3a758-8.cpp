void ha_checkpoint_state(bool disable)
{
  plugin_foreach(NULL, checkpoint_state_handlerton, MYSQL_STORAGE_ENGINE_PLUGIN, &disable);
}