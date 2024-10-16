bool ha_flush_logs(handlerton *db_type)
{
  if (db_type == NULL)
  {
    if (plugin_foreach(NULL, flush_handlerton,
                          MYSQL_STORAGE_ENGINE_PLUGIN, 0))
      return TRUE;
  }
  else
  {
    if (db_type->state != SHOW_OPTION_YES ||
        (db_type->flush_logs && db_type->flush_logs(db_type)))
      return TRUE;
  }
  return FALSE;
}