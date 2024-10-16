bool Ordered_key::sort_keys()
{
  if (tbl->file->ha_rnd_init_with_error(0))
    return TRUE;
  my_qsort2(key_buff, (size_t) key_buff_elements, sizeof(rownum_t),
            (qsort2_cmp) &cmp_keys_by_row_data_and_rownum, (void*) this);
  /* Invalidate the current row position. */
  cur_key_idx= HA_POS_ERROR;
  tbl->file->ha_rnd_end();
  return FALSE;
}