int handler::ha_index_read_idx_map(uchar *buf, uint index, const uchar *key,
                                          key_part_map keypart_map,
                                          enum ha_rkey_function find_flag)
{
  int result;
  DBUG_ASSERT(inited==NONE);
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type != F_UNLCK);
  DBUG_ASSERT(end_range == NULL);
  TABLE_IO_WAIT(tracker, m_psi, PSI_TABLE_FETCH_ROW, index, 0,
    { result= index_read_idx_map(buf, index, key, keypart_map, find_flag); })
  increment_statistics(&SSV::ha_read_key_count);
  if (!result)
  {
    update_rows_read();
    index_rows_read[index]++;
    if (table->vfield && buf == table->record[0])
      table->update_virtual_fields(this, VCOL_UPDATE_FOR_READ);
  }
  table->status=result ? STATUS_NOT_FOUND: 0;
  return result;
}