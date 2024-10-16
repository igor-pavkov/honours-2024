int handler::ha_check(THD *thd, HA_CHECK_OPT *check_opt)
{
  int error;
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type != F_UNLCK);

  if ((table->s->mysql_version >= MYSQL_VERSION_ID) &&
      (check_opt->sql_flags & TT_FOR_UPGRADE))
    return 0;

  if (table->s->mysql_version < MYSQL_VERSION_ID)
  {
    if (unlikely((error= check_old_types())))
      return error;
    error= ha_check_for_upgrade(check_opt);
    if (unlikely(error && (error != HA_ADMIN_NEEDS_CHECK)))
      return error;
    if (unlikely(!error && (check_opt->sql_flags & TT_FOR_UPGRADE)))
      return 0;
  }
  if (unlikely((error= check(thd, check_opt))))
    return error;
  /* Skip updating frm version if not main handler. */
  if (table->file != this)
    return error;
  return update_frm_version(table);
}