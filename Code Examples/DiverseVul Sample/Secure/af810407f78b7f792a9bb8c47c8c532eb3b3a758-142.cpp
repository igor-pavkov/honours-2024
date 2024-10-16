int handler::ha_direct_delete_rows(ha_rows *delete_rows)
{
  int error;
  /* Ensure we are not using binlog row */
  DBUG_ASSERT(!table->in_use->is_current_stmt_binlog_format_row());

  MYSQL_DELETE_ROW_START(table_share->db.str, table_share->table_name.str);
  mark_trx_read_write();

  error = direct_delete_rows(delete_rows);
  MYSQL_DELETE_ROW_DONE(error);
  return error;
}