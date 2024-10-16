double Ordered_key::null_selectivity()
{
  /* We should not be processing empty tables. */
  DBUG_ASSERT(tbl->file->stats.records);
  return (1 - (double) null_count / (double) tbl->file->stats.records);
}