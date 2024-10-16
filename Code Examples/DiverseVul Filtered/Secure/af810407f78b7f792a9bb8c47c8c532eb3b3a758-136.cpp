static my_bool flush_handlerton(THD *thd, plugin_ref plugin,
                                void *arg)
{
  handlerton *hton= plugin_hton(plugin);
  if (hton->state == SHOW_OPTION_YES && hton->flush_logs && 
      hton->flush_logs(hton))
    return TRUE;
  return FALSE;
}