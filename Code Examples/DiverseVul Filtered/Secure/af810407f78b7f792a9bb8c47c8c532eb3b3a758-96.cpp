int ha_discover_table(THD *thd, TABLE_SHARE *share)
{
  DBUG_ENTER("ha_discover_table");
  int found;

  DBUG_ASSERT(share->error == OPEN_FRM_OPEN_ERROR);   // share is not OK yet

  if (!engines_with_discover)
    found= FALSE;
  else if (share->db_plugin)
    found= discover_handlerton(thd, share->db_plugin, share);
  else
    found= plugin_foreach(thd, discover_handlerton,
                        MYSQL_STORAGE_ENGINE_PLUGIN, share);

  if (thd->lex->query_tables && thd->lex->query_tables->sequence && !found)
    my_error(ER_UNKNOWN_SEQUENCES, MYF(0),share->table_name.str);
  if (!found)
    open_table_error(share, OPEN_FRM_OPEN_ERROR, ENOENT); // not found

  DBUG_RETURN(share->error != OPEN_FRM_OK);
}