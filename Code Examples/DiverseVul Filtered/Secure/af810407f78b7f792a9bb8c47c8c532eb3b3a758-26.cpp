static my_bool discover_handlerton(THD *thd, plugin_ref plugin,
                                   void *arg)
{
  TABLE_SHARE *share= (TABLE_SHARE *)arg;
  handlerton *hton= plugin_hton(plugin);
  if (hton->state == SHOW_OPTION_YES && hton->discover_table)
  {
    share->db_plugin= plugin;
    int error= hton->discover_table(hton, thd, share);
    if (error != HA_ERR_NO_SUCH_TABLE)
    {
      if (unlikely(error))
      {
        if (!share->error)
        {
          share->error= OPEN_FRM_ERROR_ALREADY_ISSUED;
          plugin_unlock(0, share->db_plugin);
        }

        /*
          report an error, unless it is "generic" and a more
          specific one was already reported
        */
        if (error != HA_ERR_GENERIC || !thd->is_error())
          my_error(ER_GET_ERRNO, MYF(0), error, plugin_name(plugin)->str);
        share->db_plugin= 0;
      }
      else
        share->error= OPEN_FRM_OK;

      status_var_increment(thd->status_var.ha_discover_count);
      return TRUE; // abort the search
    }
    share->db_plugin= 0;
  }

  DBUG_ASSERT(share->error == OPEN_FRM_OPEN_ERROR);
  return FALSE;    // continue with the next engine
}