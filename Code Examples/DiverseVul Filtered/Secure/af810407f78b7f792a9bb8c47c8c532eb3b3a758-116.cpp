int handler::ha_update_row(const uchar *old_data, const uchar *new_data)
{
  int error;
  Log_func *log_func= Update_rows_log_event::binlog_row_logging_function;
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type == F_WRLCK);

  /*
    Some storage engines require that the new record is in record[0]
    (and the old record is in record[1]).
   */
  DBUG_ASSERT(new_data == table->record[0]);
  DBUG_ASSERT(old_data == table->record[1]);

  MYSQL_UPDATE_ROW_START(table_share->db.str, table_share->table_name.str);
  mark_trx_read_write();
  increment_statistics(&SSV::ha_update_count);
  if (table->s->long_unique_table &&
          (error= check_duplicate_long_entries_update(table, table->file, (uchar *)new_data)))
  {
    return error;
  }

  TABLE_IO_WAIT(tracker, m_psi, PSI_TABLE_UPDATE_ROW, active_index, 0,
                      { error= update_row(old_data, new_data);})

  MYSQL_UPDATE_ROW_DONE(error);
  if (likely(!error) && !row_already_logged)
  {
    rows_changed++;
    error= binlog_log_row(table, old_data, new_data, log_func);
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