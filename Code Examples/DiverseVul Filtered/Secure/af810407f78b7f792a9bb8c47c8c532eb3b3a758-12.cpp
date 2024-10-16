int handler::ha_rnd_next(uchar *buf)
{
  int result;
  DBUG_ENTER("handler::ha_rnd_next");
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type != F_UNLCK);
  DBUG_ASSERT(inited == RND);

  do
  {
    TABLE_IO_WAIT(tracker, m_psi, PSI_TABLE_FETCH_ROW, MAX_KEY, 0,
      { result= rnd_next(buf); })
    if (result != HA_ERR_RECORD_DELETED)
      break;
    status_var_increment(table->in_use->status_var.ha_read_rnd_deleted_count);
  } while (!table->in_use->check_killed(1));

  if (result == HA_ERR_RECORD_DELETED)
    result= HA_ERR_ABORTED_BY_USER;
  else
  {
    if (!result)
    {
      update_rows_read();
      if (table->vfield && buf == table->record[0])
        table->update_virtual_fields(this, VCOL_UPDATE_FOR_READ);
    }
    increment_statistics(&SSV::ha_read_rnd_next_count);
  }

  table->status=result ? STATUS_NOT_FOUND: 0;
  DBUG_RETURN(result);
}