handler::ha_analyze(THD* thd, HA_CHECK_OPT* check_opt)
{
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type != F_UNLCK);
  mark_trx_read_write();

  return analyze(thd, check_opt);
}