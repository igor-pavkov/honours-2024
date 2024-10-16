static int binlog_log_row_internal(TABLE* table,
                                   const uchar *before_record,
                                   const uchar *after_record,
                                   Log_func *log_func)
{
  bool error= 0;
  THD *const thd= table->in_use;

  /*
    If there are no table maps written to the binary log, this is
    the first row handled in this statement. In that case, we need
    to write table maps for all locked tables to the binary log.
  */
  if (likely(!(error= ((thd->get_binlog_table_maps() == 0 &&
                        write_locked_table_maps(thd))))))
  {
    /*
      We need to have a transactional behavior for SQLCOM_CREATE_TABLE
      (i.e. CREATE TABLE... SELECT * FROM TABLE) in order to keep a
      compatible behavior with the STMT based replication even when
      the table is not transactional. In other words, if the operation
      fails while executing the insert phase nothing is written to the
      binlog.
    */
    bool const has_trans= thd->lex->sql_command == SQLCOM_CREATE_TABLE ||
      table->file->has_transactions();
    error= (*log_func)(thd, table, has_trans, before_record, after_record);
  }
  return error ? HA_ERR_RBR_LOGGING_FAILED : 0;
}