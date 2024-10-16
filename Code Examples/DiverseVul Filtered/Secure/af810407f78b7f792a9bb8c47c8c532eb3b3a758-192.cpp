handler::ha_discard_or_import_tablespace(my_bool discard)
{
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type == F_WRLCK);
  mark_trx_read_write();

  return discard_or_import_tablespace(discard);
}