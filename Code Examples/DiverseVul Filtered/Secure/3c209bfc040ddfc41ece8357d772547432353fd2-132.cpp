bool Item_in_subselect::init_left_expr_cache()
{
  JOIN *outer_join;
  DBUG_ASSERT(thd);

  outer_join= unit->outer_select()->join;
  /*
    An IN predicate might be evaluated in a query for which all tables have
    been optimzied away.
  */ 
  if (!outer_join || !outer_join->table_count || !outer_join->tables_list)
    return TRUE;

  if (!(left_expr_cache= new List<Cached_item>))
    return TRUE;

  for (uint i= 0; i < left_expr->cols(); i++)
  {
    Cached_item *cur_item_cache= new_Cached_item(thd,
                                                 left_expr->element_index(i),
                                                 FALSE);
    if (!cur_item_cache || left_expr_cache->push_front(cur_item_cache,
                                                       thd->mem_root))
      return TRUE;
  }
  return FALSE;
}