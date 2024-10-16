static my_bool dropdb_handlerton(THD *unused1, plugin_ref plugin,
                                 void *path)
{
  handlerton *hton= plugin_hton(plugin);
  if (hton->state == SHOW_OPTION_YES && hton->drop_database)
    hton->drop_database(hton, (char *)path);
  return FALSE;
}