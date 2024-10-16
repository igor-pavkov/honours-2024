bool subselect_rowid_merge_engine::test_null_row(rownum_t row_num)
{
  Ordered_key *cur_key;
  for (uint i = 0; i < merge_keys_count; i++)
  {
    cur_key= merge_keys[i];
    if (bitmap_is_set(&matching_keys, cur_key->get_keyid()))
    {
      /*
        The key 'i' (with id 'cur_keyid') already matches a value in row
        'row_num', thus we skip it as it can't possibly match a NULL.
      */
      continue;
    }
    if (!cur_key->is_null(row_num))
      return FALSE;
  }
  return TRUE;
}