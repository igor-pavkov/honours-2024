int handler::ha_delete_row(const uchar *buf)
{
  int error;
  Log_func *log_func= Delete_rows_log_event::binlog_row_logging_function;
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type == F_WRLCK);
  /*
    Normally table->record[0] is used, but sometimes table->record[1] is used.
  */
  DBUG_ASSERT(buf == table->record[0] ||
              buf == table->record[1]);

  MYSQL_DELETE_ROW_START(table_share->db.str, table_share->table_name.str);
  mark_trx_read_write();
  increment_statistics(&SSV::ha_delete_count);

  TABLE_IO_WAIT(tracker, m_psi, PSI_TABLE_DELETE_ROW, active_index, 0,
    { error= delete_row(buf);})
  MYSQL_DELETE_ROW_DONE(error);
  if (likely(!error))
  {
    rows_changed++;
    error= binlog_log_row(table, buf, 0, log_func);
#ifdef WITH_WSREP
    THD *thd= ha_thd();
    bool is_wsrep= WSREP(thd);
    /* for SR, the followin wsrep_after_row() may replicate a fragment, so we have to
       declare potential PA unsafe before that*/
    if (table->s->primary_key == MAX_KEY &&
	is_wsrep && wsrep_thd_is_local(thd))
    {
      WSREP_DEBUG("marking trx as PA unsafe pk %d", table->s->primary_key);
      if (thd->wsrep_cs().mark_transaction_pa_unsafe())
      {
        WSREP_DEBUG("session does not have active transaction, can not mark as PA unsafe");
      }
    }
    if (table_share->tmp_table == NO_TMP_TABLE &&
        is_wsrep && ht->flags & HTON_WSREP_REPLICATION &&
        !error && (error= wsrep_after_row(thd)))
    {
      return error;
    }
#endif /* WITH_WSREP */
  }
  return error;
}