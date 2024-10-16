int handler::ha_index_next(uchar * buf)
{
  int result;
  DBUG_ENTER("handler::ha_index_next");
 DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type != F_UNLCK);
  DBUG_ASSERT(inited==INDEX);

  TABLE_IO_WAIT(tracker, m_psi, PSI_TABLE_FETCH_ROW, active_index, 0,
    { result= index_next(buf); })
  increment_statistics(&SSV::ha_read_next_count);
  if (!result)
  {
    update_index_statistics();
    if (table->vfield && buf == table->record[0])
      table->update_virtual_fields(this, VCOL_UPDATE_FOR_READ);
  }
  table->status=result ? STATUS_NOT_FOUND: 0;
  DBUG_RETURN(result);
}