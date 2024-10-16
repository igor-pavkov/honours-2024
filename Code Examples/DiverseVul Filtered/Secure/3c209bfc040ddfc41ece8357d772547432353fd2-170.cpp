ulonglong subselect_hash_sj_engine::rowid_merge_buff_size(
  bool has_non_null_key, bool has_covering_null_row,
  MY_BITMAP *partial_match_key_parts)
{
  /* Total size of all buffers used by partial matching. */
  ulonglong buff_size;
  ha_rows row_count= tmp_table->file->stats.records;
  uint rowid_length= tmp_table->file->ref_length;
  select_materialize_with_stats *result_sink=
    (select_materialize_with_stats *) result;
  ha_rows max_null_row;

  /* Size of the subselect_rowid_merge_engine::row_num_to_rowid buffer. */
  buff_size= row_count * rowid_length * sizeof(uchar);

  if (has_non_null_key)
  {
    /* Add the size of Ordered_key::key_buff of the only non-NULL key. */
    buff_size+= row_count * sizeof(rownum_t);
  }

  if (!has_covering_null_row)
  {
    for (uint i= 0; i < partial_match_key_parts->n_bits; i++)
    {
      if (!bitmap_is_set(partial_match_key_parts, i) ||
          result_sink->get_null_count_of_col(i) == row_count)
        continue; /* In these cases we wouldn't construct Ordered keys. */

      /* Add the size of Ordered_key::key_buff */
      buff_size+= (row_count - result_sink->get_null_count_of_col(i)) *
                         sizeof(rownum_t);
      /* Add the size of Ordered_key::null_key */
      max_null_row= result_sink->get_max_null_of_col(i);
      if (max_null_row >= UINT_MAX)
      {
        /*
          There can be at most UINT_MAX bits in a MY_BITMAP that is used to
          store NULLs in an Ordered_key. Return a number of bytes bigger than
          the maximum allowed memory buffer for partial matching to disable
          the rowid merge strategy.
        */
        return ULONGLONG_MAX;
      }
      buff_size+= bitmap_buffer_size(max_null_row);
    }
  }

  return buff_size;
}