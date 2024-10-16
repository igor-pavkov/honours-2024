plugin_ref ha_lock_engine(THD *thd, const handlerton *hton)
{
  if (hton)
  {
    st_plugin_int *plugin= hton2plugin[hton->slot];
    return my_plugin_lock(thd, plugin_int_to_ref(plugin));
  }
  return NULL;
}