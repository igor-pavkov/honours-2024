int handler::ha_write_row(const uchar *buf)
{
  int error;
  Log_func *log_func= Write_rows_log_event::binlog_row_logging_function;
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type == F_WRLCK);
  DBUG_ENTER("handler::ha_write_row");
  DEBUG_SYNC_C("ha_write_row_start");

  MYSQL_INSERT_ROW_START(table_share->db.str, table_share->table_name.str);
  mark_trx_read_write();
  increment_statistics(&SSV::ha_write_count);

  if (table->s->long_unique_table)
  {
    if (this->inited == RND)
      table->clone_handler_for_update();
    handler *h= table->update_handler ? table->update_handler : table->file;
    if ((error= check_duplicate_long_entries(table, h, buf)))
      DBUG_RETURN(error);
  }
  TABLE_IO_WAIT(tracker, m_psi, PSI_TABLE_WRITE_ROW, MAX_KEY, 0,
                      { error= write_row(buf); })

  MYSQL_INSERT_ROW_DONE(error);
  if (likely(!error) && !row_already_logged)
  {
    rows_changed++;
    error= binlog_log_row(table, 0, buf, log_func);
#ifdef WITH_WSREP
    if (table_share->tmp_table == NO_TMP_TABLE &&
        WSREP(ha_thd()) && ht->flags & HTON_WSREP_REPLICATION &&
        !error && (error= wsrep_after_row(ha_thd())))
    {
      DBUG_RETURN(error);
    }
#endif /* WITH_WSREP */
  }

  DEBUG_SYNC_C("ha_write_row_end");
  DBUG_RETURN(error);
}