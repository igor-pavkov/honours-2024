Storage_engine_name::resolve_storage_engine_with_error(THD *thd,
                                                       handlerton **ha,
                                                       bool tmp_table)
{
  if (plugin_ref plugin= ha_resolve_by_name(thd, &m_storage_engine_name,
                                            tmp_table))
  {
    *ha= plugin_hton(plugin);
    return false;
  }

  *ha= NULL;
  if (thd->variables.sql_mode & MODE_NO_ENGINE_SUBSTITUTION)
  {
    my_error(ER_UNKNOWN_STORAGE_ENGINE, MYF(0), m_storage_engine_name.str);
    return true;
  }
  push_warning_printf(thd, Sql_condition::WARN_LEVEL_WARN,
                      ER_UNKNOWN_STORAGE_ENGINE,
                      ER_THD(thd, ER_UNKNOWN_STORAGE_ENGINE),
                      m_storage_engine_name.str);
  return false;
}