static my_bool showstat_handlerton(THD *thd, plugin_ref plugin,
                                   void *arg)
{
  enum ha_stat_type stat= *(enum ha_stat_type *) arg;
  handlerton *hton= plugin_hton(plugin);
  if (hton->state == SHOW_OPTION_YES && hton->show_status &&
      hton->show_status(hton, thd, stat_print, stat))
    return TRUE;
  return FALSE;
}