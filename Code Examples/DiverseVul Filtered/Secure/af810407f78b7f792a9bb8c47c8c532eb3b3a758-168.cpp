int handler::ha_direct_update_rows(ha_rows *update_rows, ha_rows *found_rows)
{
  int error;

  MYSQL_UPDATE_ROW_START(table_share->db.str, table_share->table_name.str);
  mark_trx_read_write();

  error = direct_update_rows(update_rows, found_rows);
  MYSQL_UPDATE_ROW_DONE(error);
  return error;
}