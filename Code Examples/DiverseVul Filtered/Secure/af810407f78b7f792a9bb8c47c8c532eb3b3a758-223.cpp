int ha_finalize_handlerton(st_plugin_int *plugin)
{
  handlerton *hton= (handlerton *)plugin->data;
  DBUG_ENTER("ha_finalize_handlerton");

  /* hton can be NULL here, if ha_initialize_handlerton() failed. */
  if (!hton)
    goto end;

  switch (hton->state) {
  case SHOW_OPTION_NO:
  case SHOW_OPTION_DISABLED:
    break;
  case SHOW_OPTION_YES:
    if (installed_htons[hton->db_type] == hton)
      installed_htons[hton->db_type]= NULL;
    break;
  };

  if (hton->panic)
    hton->panic(hton, HA_PANIC_CLOSE);

  if (plugin->plugin->deinit)
  {
    /*
      Today we have no defined/special behavior for uninstalling
      engine plugins.
    */
    DBUG_PRINT("info", ("Deinitializing plugin: '%s'", plugin->name.str));
    if (plugin->plugin->deinit(NULL))
    {
      DBUG_PRINT("warning", ("Plugin '%s' deinit function returned error.",
                             plugin->name.str));
    }
  }

  free_sysvar_table_options(hton);
  update_discovery_counters(hton, -1);

  /*
    In case a plugin is uninstalled and re-installed later, it should
    reuse an array slot. Otherwise the number of uninstall/install
    cycles would be limited.
  */
  if (hton->slot != HA_SLOT_UNDEF)
  {
    /* Make sure we are not unpluging another plugin */
    DBUG_ASSERT(hton2plugin[hton->slot] == plugin);
    DBUG_ASSERT(hton->slot < MAX_HA);
    hton2plugin[hton->slot]= NULL;
  }

  my_free(hton);

 end:
  DBUG_RETURN(0);
}