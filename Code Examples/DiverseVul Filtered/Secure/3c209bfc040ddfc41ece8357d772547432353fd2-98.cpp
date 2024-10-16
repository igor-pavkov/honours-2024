int subselect_uniquesubquery_engine::prepare(THD *)
{
  /* Should never be called. */
  DBUG_ASSERT(FALSE);
  return 1;
}