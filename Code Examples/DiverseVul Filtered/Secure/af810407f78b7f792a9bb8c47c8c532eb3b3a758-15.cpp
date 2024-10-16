static my_bool closecon_handlerton(THD *thd, plugin_ref plugin,
                                   void *unused)
{
  handlerton *hton= plugin_hton(plugin);
  /*
    there's no need to rollback here as all transactions must
    be rolled back already
  */
  if (hton->state == SHOW_OPTION_YES && thd_get_ha_data(thd, hton))
  {
    if (hton->close_connection)
      hton->close_connection(hton, thd);
    /* make sure ha_data is reset and ha_data_lock is released */
    thd_set_ha_data(thd, hton, NULL);
  }
  return FALSE;
}