handlerton *ha_resolve_by_legacy_type(THD *thd, enum legacy_db_type db_type)
{
  plugin_ref plugin;
  switch (db_type) {
  case DB_TYPE_DEFAULT:
    return ha_default_handlerton(thd);
  default:
    if (db_type > DB_TYPE_UNKNOWN && db_type < DB_TYPE_DEFAULT &&
        (plugin= ha_lock_engine(thd, installed_htons[db_type])))
      return plugin_hton(plugin);
    /* fall through */
  case DB_TYPE_UNKNOWN:
    return NULL;
  }
}