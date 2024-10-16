bool Ordered_key::next_same()
{
  DBUG_ASSERT(key_buff_elements);

  if (cur_key_idx < key_buff_elements - 1)
  {
    /*
      TIMOUR:
      The below is quite inefficient, since as a result we will fetch every
      row (except the last one) twice. There must be a more efficient way,
      e.g. swapping record[0] and record[1], and reading only the new record.
    */
    if (!cmp_keys_by_row_data(key_buff[cur_key_idx], key_buff[cur_key_idx + 1]))
    {
      ++cur_key_idx;
      return TRUE;
    }
  }
  return FALSE;
}