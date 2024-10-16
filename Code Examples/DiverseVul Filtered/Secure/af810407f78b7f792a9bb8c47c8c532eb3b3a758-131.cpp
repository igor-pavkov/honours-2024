int handler::ha_rnd_pos(uchar *buf, uchar *pos)
{
  int result;
  DBUG_ENTER("handler::ha_rnd_pos");
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type != F_UNLCK);
  DBUG_ASSERT(inited == RND);

  TABLE_IO_WAIT(tracker, m_psi, PSI_TABLE_FETCH_ROW, MAX_KEY, 0,
    { result= rnd_pos(buf, pos); })
  increment_statistics(&SSV::ha_read_rnd_count);
  if (result == HA_ERR_RECORD_DELETED)
    result= HA_ERR_KEY_NOT_FOUND;
  else if (!result)
  {
    update_rows_read();
    if (table->vfield && buf == table->record[0])
      table->update_virtual_fields(this, VCOL_UPDATE_FOR_READ);
  }
  table->status=result ? STATUS_NOT_FOUND: 0;
  DBUG_RETURN(result);
}