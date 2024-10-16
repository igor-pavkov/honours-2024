handler::ha_optimize(THD* thd, HA_CHECK_OPT* check_opt)
{
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type == F_WRLCK);
  mark_trx_read_write();

  return optimize(thd, check_opt);
}