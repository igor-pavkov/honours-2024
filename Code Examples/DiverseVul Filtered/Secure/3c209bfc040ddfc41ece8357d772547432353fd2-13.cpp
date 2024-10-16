subselect_hash_sj_engine::choose_partial_match_strategy(
  bool has_non_null_key, bool has_covering_null_row,
  MY_BITMAP *partial_match_key_parts_arg)
{
  ulonglong pm_buff_size;

  DBUG_ASSERT(strategy == PARTIAL_MATCH);
  /*
    Choose according to global optimizer switch. If only one of the switches is
    'ON', then the remaining strategy is the only possible one. The only cases
    when this will be overridden is when the total size of all buffers for the
    merge strategy is bigger than the 'rowid_merge_buff_size' system variable,
    or if there isn't enough physical memory to allocate the buffers.
  */
  if (!optimizer_flag(thd, OPTIMIZER_SWITCH_PARTIAL_MATCH_ROWID_MERGE) &&
       optimizer_flag(thd, OPTIMIZER_SWITCH_PARTIAL_MATCH_TABLE_SCAN))
    strategy= PARTIAL_MATCH_SCAN;
  else if
     ( optimizer_flag(thd, OPTIMIZER_SWITCH_PARTIAL_MATCH_ROWID_MERGE) &&
      !optimizer_flag(thd, OPTIMIZER_SWITCH_PARTIAL_MATCH_TABLE_SCAN))
    strategy= PARTIAL_MATCH_MERGE;

  /*
    If both switches are ON, or both are OFF, we interpret that as "let the
    optimizer decide". Perform a cost based choice between the two partial
    matching strategies.
  */
  /*
    TIMOUR: the above interpretation of the switch values could be changed to:
    - if both are ON - let the optimizer decide,
    - if both are OFF - do not use partial matching, therefore do not use
      materialization in non-top-level predicates.
    The problem with this is that we know for sure if we need partial matching
    only after the subquery is materialized, and this is too late to revert to
    the IN=>EXISTS strategy.
  */
  if (strategy == PARTIAL_MATCH)
  {
    /*
      TIMOUR: Currently we use a super simplistic measure. This will be
      addressed in a separate task.
    */
    if (tmp_table->file->stats.records < 100)
      strategy= PARTIAL_MATCH_SCAN;
    else
      strategy= PARTIAL_MATCH_MERGE;
  }

  /* Check if there is enough memory for the rowid merge strategy. */
  if (strategy == PARTIAL_MATCH_MERGE)
  {
    pm_buff_size= rowid_merge_buff_size(has_non_null_key,
                                        has_covering_null_row,
                                        partial_match_key_parts_arg);
    if (pm_buff_size > thd->variables.rowid_merge_buff_size)
      strategy= PARTIAL_MATCH_SCAN;
  }
}