bool Ordered_key::alloc_keys_buffers()
{
  DBUG_ASSERT(key_buff_elements > 0);

  if (!(key_buff= (rownum_t*) my_malloc((size_t)(key_buff_elements * 
    sizeof(rownum_t)), MYF(MY_WME | MY_THREAD_SPECIFIC))))
    return TRUE;

  /*
    TIMOUR: it is enough to create bitmaps with size
    (max_null_row - min_null_row), and then use min_null_row as
    lookup offset.
  */
  /* Notice that max_null_row is max array index, we need count, so +1. */
  if (my_bitmap_init(&null_key, NULL, (uint)(max_null_row + 1), FALSE))
    return TRUE;

  cur_key_idx= HA_POS_ERROR;

  return FALSE;
}