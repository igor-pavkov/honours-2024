bool ha_table_exists(THD *thd, const LEX_CSTRING *db, const LEX_CSTRING *table_name,
                     handlerton **hton, bool *is_sequence)
{
  handlerton *dummy;
  bool dummy2;
  DBUG_ENTER("ha_table_exists");

  if (hton)
    *hton= 0;
  else if (engines_with_discover)
    hton= &dummy;
  if (!is_sequence)
    is_sequence= &dummy2;
  *is_sequence= 0;

  TDC_element *element= tdc_lock_share(thd, db->str, table_name->str);
  if (element && element != MY_ERRPTR)
  {
    if (hton)
      *hton= element->share->db_type();
    *is_sequence= element->share->table_type == TABLE_TYPE_SEQUENCE;
    tdc_unlock_share(element);
    DBUG_RETURN(TRUE);
  }

  char path[FN_REFLEN + 1];
  size_t path_len = build_table_filename(path, sizeof(path) - 1,
                                         db->str, table_name->str, "", 0);
  st_discover_existence_args args= {path, path_len, db->str, table_name->str, 0, true};

  if (file_ext_exists(path, path_len, reg_ext))
  {
    bool exists= true;
    if (hton)
    {
      char engine_buf[NAME_CHAR_LEN + 1];
      LEX_CSTRING engine= { engine_buf, 0 };
      Table_type type;

      if ((type= dd_frm_type(thd, path, &engine, is_sequence)) ==
          TABLE_TYPE_UNKNOWN)
        DBUG_RETURN(0);
      
      if (type != TABLE_TYPE_VIEW)
      {
        plugin_ref p=  plugin_lock_by_name(thd, &engine,
                                           MYSQL_STORAGE_ENGINE_PLUGIN);
        *hton= p ? plugin_hton(p) : NULL;
        if (*hton)
          // verify that the table really exists
          exists= discover_existence(thd, p, &args);
      }
      else
        *hton= view_pseudo_hton;
    }
    DBUG_RETURN(exists);
  }

  args.frm_exists= false;
  if (plugin_foreach(thd, discover_existence, MYSQL_STORAGE_ENGINE_PLUGIN,
                     &args))
  {
    if (hton)
      *hton= args.hton;
    DBUG_RETURN(TRUE);
  }

  if (need_full_discover_for_existence)
  {
    TABLE_LIST table;
    uint flags = GTS_TABLE | GTS_VIEW;
    if (!hton)
      flags|= GTS_NOLOCK;

    Table_exists_error_handler no_such_table_handler;
    thd->push_internal_handler(&no_such_table_handler);
    table.init_one_table(db, table_name, 0, TL_READ);
    TABLE_SHARE *share= tdc_acquire_share(thd, &table, flags);
    thd->pop_internal_handler();

    if (hton && share)
    {
      *hton= share->db_type();
      tdc_release_share(share);
    }

    // the table doesn't exist if we've caught ER_NO_SUCH_TABLE and nothing else
    DBUG_RETURN(!no_such_table_handler.safely_trapped_errors());
  }

  DBUG_RETURN(FALSE);
}