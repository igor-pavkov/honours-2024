handler::ha_disable_indexes(uint mode)
{
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type != F_UNLCK);
  mark_trx_read_write();

  return disable_indexes(mode);
}