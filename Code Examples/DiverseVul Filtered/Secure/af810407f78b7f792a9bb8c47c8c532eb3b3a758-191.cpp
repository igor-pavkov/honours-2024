static my_bool plugin_prepare_for_backup(THD *unused1, plugin_ref plugin,
                                         void *not_used)
{
  handlerton *hton= plugin_hton(plugin);
  if (hton->state == SHOW_OPTION_YES && hton->prepare_for_backup)
    hton->prepare_for_backup();
  return FALSE;
}