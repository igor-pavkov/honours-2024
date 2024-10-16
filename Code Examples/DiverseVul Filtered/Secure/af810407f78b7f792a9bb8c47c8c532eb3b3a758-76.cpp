handlerton *ha_default_tmp_handlerton(THD *thd)
{
  plugin_ref plugin= ha_default_tmp_plugin(thd);
  DBUG_ASSERT(plugin);
  handlerton *hton= plugin_hton(plugin);
  DBUG_ASSERT(hton);
  return hton;
}