bool subselect_single_select_engine::may_be_null()
{
  return ((no_tables() && !join->conds && !join->having) ? maybe_null : 1);
}