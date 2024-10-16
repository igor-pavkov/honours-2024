int handler::ha_reset()
{
  DBUG_ENTER("ha_reset");
  /* Check that we have called all proper deallocation functions */
  DBUG_ASSERT((uchar*) table->def_read_set.bitmap +
              table->s->column_bitmap_size ==
              (uchar*) table->def_write_set.bitmap);
  DBUG_ASSERT(bitmap_is_set_all(&table->s->all_set));
  DBUG_ASSERT(!table->file->keyread_enabled());
  /* ensure that ha_index_end / ha_rnd_end has been called */
  DBUG_ASSERT(inited == NONE);
  /* reset the bitmaps to point to defaults */
  table->default_column_bitmaps();
  pushed_cond= NULL;
  tracker= NULL;
  mark_trx_read_write_done= 0;
  clear_cached_table_binlog_row_based_flag();
  /* Reset information about pushed engine conditions */
  cancel_pushed_idx_cond();
  /* Reset information about pushed index conditions */
  cancel_pushed_rowid_filter();
  clear_top_table_fields();
  DBUG_RETURN(reset());
}