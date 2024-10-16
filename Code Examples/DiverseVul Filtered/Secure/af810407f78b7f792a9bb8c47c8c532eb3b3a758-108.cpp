bool handler::check_table_binlog_row_based(bool binlog_row)
{
  if (table->versioned(VERS_TRX_ID))
    return false;
  if (unlikely((table->in_use->variables.sql_log_bin_off)))
    return 0;                            /* Called by partitioning engine */
#ifdef WITH_WSREP
  if (!table->in_use->variables.sql_log_bin &&
      wsrep_thd_is_applying(table->in_use))
    return 0;      /* wsrep patch sets sql_log_bin to silence binlogging
                      from high priority threads */
#endif /* WITH_WSREP */
  if (unlikely((!check_table_binlog_row_based_done)))
  {
    check_table_binlog_row_based_done= 1;
    check_table_binlog_row_based_result=
      check_table_binlog_row_based_internal(binlog_row);
  }
  return check_table_binlog_row_based_result;
}