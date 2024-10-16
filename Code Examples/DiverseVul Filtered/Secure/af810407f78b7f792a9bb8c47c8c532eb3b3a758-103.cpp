handler::ha_delete_all_rows()
{
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type == F_WRLCK);
  mark_trx_read_write();

  return delete_all_rows();
}