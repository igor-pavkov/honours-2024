bool subselect_rowid_merge_engine::
exists_complementing_null_row(MY_BITMAP *keys_to_complement)
{
  rownum_t highest_min_row= 0;
  rownum_t lowest_max_row= UINT_MAX;
  uint count_null_keys, i;
  Ordered_key *cur_key;

  if (!count_columns_with_nulls)
  {
    /*
      If there are both NULLs and non-NUll values in the outer reference, and
      the subquery contains no NULLs, a complementing NULL row cannot exist.
    */
    return FALSE;
  }

  for (i= (non_null_key ? 1 : 0), count_null_keys= 0; i < merge_keys_count; i++)
  {
    cur_key= merge_keys[i];
    if (bitmap_is_set(keys_to_complement, cur_key->get_keyid()))
      continue;
    if (!cur_key->get_null_count())
    {
      /* If there is column without NULLs, there cannot be a partial match. */
      return FALSE;
    }
    if (cur_key->get_min_null_row() > highest_min_row)
      highest_min_row= cur_key->get_min_null_row();
    if (cur_key->get_max_null_row() < lowest_max_row)
      lowest_max_row= cur_key->get_max_null_row();
    null_bitmaps[count_null_keys++]= cur_key->get_null_key();
  }

  if (lowest_max_row < highest_min_row)
  {
    /* The intersection of NULL rows is empty. */
    return FALSE;
  }

  return bitmap_exists_intersection((const MY_BITMAP**) null_bitmaps,
                                    count_null_keys,
                                    (uint)highest_min_row, (uint)lowest_max_row);
}