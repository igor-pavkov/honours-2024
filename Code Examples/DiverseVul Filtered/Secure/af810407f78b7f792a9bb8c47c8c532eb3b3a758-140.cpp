plugin_ref ha_resolve_by_name(THD *thd, const LEX_CSTRING *name,
                              bool tmp_table)
{
  const LEX_CSTRING *table_alias;
  plugin_ref plugin;

redo:
  /* my_strnncoll is a macro and gcc doesn't do early expansion of macro */
  if (thd && !my_charset_latin1.coll->strnncoll(&my_charset_latin1,
                           (const uchar *)name->str, name->length,
                           (const uchar *)STRING_WITH_LEN("DEFAULT"), 0))
    return tmp_table ?  ha_default_tmp_plugin(thd) : ha_default_plugin(thd);

  if ((plugin= my_plugin_lock_by_name(thd, name, MYSQL_STORAGE_ENGINE_PLUGIN)))
  {
    handlerton *hton= plugin_hton(plugin);
    if (hton && !(hton->flags & HTON_NOT_USER_SELECTABLE))
      return plugin;
      
    /*
      unlocking plugin immediately after locking is relatively low cost.
    */
    plugin_unlock(thd, plugin);
  }

  /*
    We check for the historical aliases.
  */
  for (table_alias= sys_table_aliases; table_alias->str; table_alias+= 2)
  {
    if (!my_strnncoll(&my_charset_latin1,
                      (const uchar *)name->str, name->length,
                      (const uchar *)table_alias->str, table_alias->length))
    {
      name= table_alias + 1;
      goto redo;
    }
  }

  return NULL;
}