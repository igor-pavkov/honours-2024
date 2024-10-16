bool Item_allany_subselect::is_maxmin_applicable(JOIN *join)
{
  /*
    Check if max/min optimization applicable: It is top item of
    WHERE condition.
  */
  return (abort_on_null || (upper_item && upper_item->is_top_level_item())) &&
      !(join->select_lex->master_unit()->uncacheable & ~UNCACHEABLE_EXPLAIN) && !func->eqne_op();
}