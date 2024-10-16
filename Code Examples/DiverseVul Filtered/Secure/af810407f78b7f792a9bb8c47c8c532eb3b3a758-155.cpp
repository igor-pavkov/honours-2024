int ha_initialize_handlerton(st_plugin_int *plugin)
{
  handlerton *hton;
  static const char *no_exts[]= { 0 };
  DBUG_ENTER("ha_initialize_handlerton");
  DBUG_PRINT("plugin", ("initialize plugin: '%s'", plugin->name.str));

  hton= (handlerton *)my_malloc(sizeof(handlerton),
                                MYF(MY_WME | MY_ZEROFILL));
  if (hton == NULL)
  {
    sql_print_error("Unable to allocate memory for plugin '%s' handlerton.",
                    plugin->name.str);
    goto err_no_hton_memory;
  }

  hton->tablefile_extensions= no_exts;
  hton->discover_table_names= hton_ext_based_table_discovery;

  hton->slot= HA_SLOT_UNDEF;
  /* Historical Requirement */
  plugin->data= hton; // shortcut for the future
  if (plugin->plugin->init && plugin->plugin->init(hton))
  {
    sql_print_error("Plugin '%s' init function returned error.",
		    plugin->name.str);
    goto err;
  }

  // hton_ext_based_table_discovery() works only when discovery
  // is supported and the engine if file-based.
  if (hton->discover_table_names == hton_ext_based_table_discovery &&
      (!hton->discover_table || !hton->tablefile_extensions[0]))
    hton->discover_table_names= NULL;

  // default discover_table_existence implementation
  if (!hton->discover_table_existence && hton->discover_table)
  {
    if (hton->tablefile_extensions[0])
      hton->discover_table_existence= ext_based_existence;
    else
      hton->discover_table_existence= full_discover_for_existence;
  }

  switch (hton->state) {
  case SHOW_OPTION_NO:
    break;
  case SHOW_OPTION_YES:
    {
      uint tmp;
      ulong fslot;

      DBUG_EXECUTE_IF("unstable_db_type", {
                        static int i= (int) DB_TYPE_FIRST_DYNAMIC;
                        hton->db_type= (enum legacy_db_type)++i;
                      });

      /* now check the db_type for conflict */
      if (hton->db_type <= DB_TYPE_UNKNOWN ||
          hton->db_type >= DB_TYPE_DEFAULT ||
          installed_htons[hton->db_type])
      {
        int idx= (int) DB_TYPE_FIRST_DYNAMIC;

        while (idx < (int) DB_TYPE_DEFAULT && installed_htons[idx])
          idx++;

        if (idx == (int) DB_TYPE_DEFAULT)
        {
          sql_print_warning("Too many storage engines!");
	  goto err_deinit;
        }
        if (hton->db_type != DB_TYPE_UNKNOWN)
          sql_print_warning("Storage engine '%s' has conflicting typecode. "
                            "Assigning value %d.", plugin->plugin->name, idx);
        hton->db_type= (enum legacy_db_type) idx;
      }

      /*
        In case a plugin is uninstalled and re-installed later, it should
        reuse an array slot. Otherwise the number of uninstall/install
        cycles would be limited. So look for a free slot.
      */
      DBUG_PRINT("plugin", ("total_ha: %lu", total_ha));
      for (fslot= 0; fslot < total_ha; fslot++)
      {
        if (!hton2plugin[fslot])
          break;
      }
      if (fslot < total_ha)
        hton->slot= fslot;
      else
      {
        if (total_ha >= MAX_HA)
        {
          sql_print_error("Too many plugins loaded. Limit is %lu. "
                          "Failed on '%s'", (ulong) MAX_HA, plugin->name.str);
          goto err_deinit;
        }
        hton->slot= total_ha++;
      }
      installed_htons[hton->db_type]= hton;
      tmp= hton->savepoint_offset;
      hton->savepoint_offset= savepoint_alloc_size;
      savepoint_alloc_size+= tmp;
      hton2plugin[hton->slot]=plugin;
      if (hton->prepare)
      {
        total_ha_2pc++;
        if (tc_log && tc_log != get_tc_log_implementation())
        {
          total_ha_2pc--;
          hton->prepare= 0;
          push_warning_printf(current_thd, Sql_condition::WARN_LEVEL_WARN,
                              ER_UNKNOWN_ERROR,
                              "Cannot enable tc-log at run-time. "
                              "XA features of %s are disabled",
                              plugin->name.str);
        }
      }
      break;
    }
    /* fall through */
  default:
    hton->state= SHOW_OPTION_DISABLED;
    break;
  }
  
  /* 
    This is entirely for legacy. We will create a new "disk based" hton and a 
    "memory" hton which will be configurable longterm. We should be able to 
    remove partition.
  */
  switch (hton->db_type) {
  case DB_TYPE_HEAP:
    heap_hton= hton;
    break;
  case DB_TYPE_MYISAM:
    myisam_hton= hton;
    break;
  case DB_TYPE_PARTITION_DB:
    partition_hton= hton;
    break;
  case DB_TYPE_SEQUENCE:
    sql_sequence_hton= hton;
    break;
  default:
    break;
  };

  resolve_sysvar_table_options(hton);
  update_discovery_counters(hton, 1);

  DBUG_RETURN(0);

err_deinit:
  /* 
    Let plugin do its inner deinitialization as plugin->init() 
    was successfully called before.
  */
  if (plugin->plugin->deinit)
    (void) plugin->plugin->deinit(NULL);
          
err:
#ifdef DBUG_ASSERT_EXISTS
  if (hton->prepare && hton->state == SHOW_OPTION_YES)
    failed_ha_2pc++;
#endif
  my_free(hton);
err_no_hton_memory:
  plugin->data= NULL;
  DBUG_RETURN(1);
}