handler::ha_bulk_update_row(const uchar *old_data, const uchar *new_data,
                            ha_rows *dup_key_found)
{
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type == F_WRLCK);
  mark_trx_read_write();

  return bulk_update_row(old_data, new_data, dup_key_found);
}