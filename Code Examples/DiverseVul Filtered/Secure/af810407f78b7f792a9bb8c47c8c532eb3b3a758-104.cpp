handler::ha_create(const char *name, TABLE *form, HA_CREATE_INFO *info_arg)
{
  DBUG_ASSERT(m_lock_type == F_UNLCK);
  mark_trx_read_write();
  int error= create(name, form, info_arg);
  if (!error &&
      !(info_arg->options & (HA_LEX_CREATE_TMP_TABLE | HA_CREATE_TMP_ALTER)))
    mysql_audit_create_table(form);
  return error;
}