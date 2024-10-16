static my_bool snapshot_handlerton(THD *thd, plugin_ref plugin,
                                   void *arg)
{
  handlerton *hton= plugin_hton(plugin);
  if (hton->state == SHOW_OPTION_YES &&
      hton->start_consistent_snapshot)
  {
    if (hton->start_consistent_snapshot(hton, thd))
      return TRUE;
    *((bool *)arg)= false;
  }
  return FALSE;
}