bool subselect_uniquesubquery_engine::no_tables()
{
  /* returning value is correct, but this method should never be called */
  DBUG_ASSERT(FALSE);
  return 0;
}