static my_bool plugin_end_backup(THD *unused1, plugin_ref plugin,
                                 void *not_used)
{
  handlerton *hton= plugin_hton(plugin);
  if (hton->state == SHOW_OPTION_YES && hton->end_backup)
    hton->end_backup();
  return FALSE;
}