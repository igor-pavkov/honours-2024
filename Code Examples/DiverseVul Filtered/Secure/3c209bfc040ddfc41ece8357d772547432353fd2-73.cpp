Ordered_key::Ordered_key(uint keyid_arg, TABLE *tbl_arg, Item *search_key_arg,
                         ha_rows null_count_arg, ha_rows min_null_row_arg,
                         ha_rows max_null_row_arg, uchar *row_num_to_rowid_arg)
  : keyid(keyid_arg), tbl(tbl_arg), search_key(search_key_arg),
    row_num_to_rowid(row_num_to_rowid_arg), null_count(null_count_arg)
{
  DBUG_ASSERT(tbl->file->stats.records > null_count);
  key_buff_elements= tbl->file->stats.records - null_count;
  cur_key_idx= HA_POS_ERROR;

  DBUG_ASSERT((null_count && min_null_row_arg && max_null_row_arg) ||
              (!null_count && !min_null_row_arg && !max_null_row_arg));
  if (null_count)
  {
    /* The counters are 1-based, for key access we need 0-based indexes. */
    min_null_row= min_null_row_arg - 1;
    max_null_row= max_null_row_arg - 1;
  }
  else
    min_null_row= max_null_row= 0;
}