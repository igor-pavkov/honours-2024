handler::ha_reset_auto_increment(ulonglong value)
{
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type == F_WRLCK);
  mark_trx_read_write();

  return reset_auto_increment(value);
}