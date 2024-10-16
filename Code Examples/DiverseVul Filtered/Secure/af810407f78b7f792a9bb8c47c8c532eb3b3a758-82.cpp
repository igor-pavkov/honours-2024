int binlog_log_row(TABLE* table, const uchar *before_record,
                   const uchar *after_record, Log_func *log_func)
{
#ifdef WITH_WSREP
  THD *const thd= table->in_use;

  /* only InnoDB tables will be replicated through binlog emulation */
  if ((WSREP_EMULATE_BINLOG(thd) &&
       !(table->file->partition_ht()->flags & HTON_WSREP_REPLICATION)) ||
      thd->wsrep_ignore_table == true)
    return 0;
#endif

  if (!table->file->check_table_binlog_row_based(1))
    return 0;
  return binlog_log_row_internal(table, before_record, after_record, log_func);
}