handler::ha_check_and_repair(THD *thd)
{
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type == F_UNLCK);
  mark_trx_read_write();

  return check_and_repair(thd);
}