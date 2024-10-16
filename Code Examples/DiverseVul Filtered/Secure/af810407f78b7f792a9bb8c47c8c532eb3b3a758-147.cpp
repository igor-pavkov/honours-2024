bool ha_show_status(THD *thd, handlerton *db_type, enum ha_stat_type stat)
{
  List<Item> field_list;
  Protocol *protocol= thd->protocol;
  MEM_ROOT *mem_root= thd->mem_root;
  bool result;

  field_list.push_back(new (mem_root) Item_empty_string(thd, "Type", 10),
                       mem_root);
  field_list.push_back(new (mem_root)
                       Item_empty_string(thd, "Name", FN_REFLEN), mem_root);
  field_list.push_back(new (mem_root)
                       Item_empty_string(thd, "Status", 10),
                       mem_root);

  if (protocol->send_result_set_metadata(&field_list,
                            Protocol::SEND_NUM_ROWS | Protocol::SEND_EOF))
    return TRUE;

  if (db_type == NULL)
  {
    result= plugin_foreach(thd, showstat_handlerton,
                           MYSQL_STORAGE_ENGINE_PLUGIN, &stat);
  }
  else
  {
    if (db_type->state != SHOW_OPTION_YES)
    {
      const LEX_CSTRING *name= hton_name(db_type);
      result= stat_print(thd, name->str, name->length,
                         "", 0, "DISABLED", 8) ? 1 : 0;
    }
    else
    {
      result= db_type->show_status &&
              db_type->show_status(db_type, thd, stat_print, stat) ? 1 : 0;
    }
  }

  /*
    We also check thd->is_error() as Innodb may return 0 even if
    there was an error.
  */
  if (likely(!result && !thd->is_error()))
    my_eof(thd);
  else if (!thd->is_error())
    my_error(ER_GET_ERRNO, MYF(0), errno, hton_name(db_type)->str);
  return result;
}