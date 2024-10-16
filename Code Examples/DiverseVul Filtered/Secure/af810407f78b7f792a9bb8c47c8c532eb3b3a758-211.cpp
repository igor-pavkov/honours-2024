bool handler::check_table_binlog_row_based_internal(bool binlog_row)
{
  THD *thd= table->in_use;

  return (table->s->can_do_row_logging &&
          thd->is_current_stmt_binlog_format_row() &&
          /*
            Wsrep partially enables binary logging if it have not been
            explicitly turned on. As a result we return 'true' if we are in
            wsrep binlog emulation mode and the current thread is not a wsrep
            applier or replayer thread. This decision is not affected by
            @@sql_log_bin as we want the events to make into the binlog
            cache only to filter them later before they make into binary log
            file.

            However, we do return 'false' if binary logging was temporarily
            turned off (see tmp_disable_binlog(A)).

            Otherwise, return 'true' if binary logging is on.
          */
          IF_WSREP(((WSREP_EMULATE_BINLOG(thd) &&
                     wsrep_thd_is_local(thd)) ||
                    ((WSREP(thd) ||
                      (thd->variables.option_bits & OPTION_BIN_LOG)) &&
                     mysql_bin_log.is_open())),
                    (thd->variables.option_bits & OPTION_BIN_LOG) &&
                    mysql_bin_log.is_open()));
}