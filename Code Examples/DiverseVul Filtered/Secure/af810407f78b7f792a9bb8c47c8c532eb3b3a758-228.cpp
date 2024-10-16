static my_bool kill_handlerton(THD *thd, plugin_ref plugin,
                               void *level)
{
  handlerton *hton= plugin_hton(plugin);

  mysql_mutex_assert_owner(&thd->LOCK_thd_kill);
  if (hton->state == SHOW_OPTION_YES && hton->kill_query &&
      thd_get_ha_data(thd, hton))
    hton->kill_query(hton, thd, *(enum thd_kill_levels *) level);
  return FALSE;
}