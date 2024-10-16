bool Item_subselect::is_expensive()
{
  double examined_rows= 0;
  bool all_are_simple= true;

  /* check extremely simple select */
  if (!unit->first_select()->next_select()) // no union
  {
    /*
      such single selects works even without optimization because
      can not makes loops
    */
    SELECT_LEX *sl= unit->first_select();
    JOIN *join = sl->join;
    if (join && !join->tables_list && !sl->first_inner_unit())
      return false;
  }


  for (SELECT_LEX *sl= unit->first_select(); sl; sl= sl->next_select())
  {
    JOIN *cur_join= sl->join;

    /* not optimized subquery */
    if (!cur_join)
      return true;

    /*
      If the subquery is not optimised or in the process of optimization
      it supposed to be expensive
    */
    if (cur_join->optimization_state != JOIN::OPTIMIZATION_DONE)
      return true;

    if (!cur_join->tables_list && !sl->first_inner_unit())
      continue;

    /*
      Subqueries whose result is known after optimization are not expensive.
      Such subqueries have all tables optimized away, thus have no join plan.
    */
    if ((cur_join->zero_result_cause || !cur_join->tables_list))
      continue;

    /*
      This is not simple SELECT in union so we can not go by simple condition
    */
    all_are_simple= false;

    /*
      If a subquery is not optimized we cannot estimate its cost. A subquery is
      considered optimized if it has a join plan.
    */
    if (!cur_join->join_tab)
      return true;

    if (sl->first_inner_unit())
    {
      /*
        Subqueries that contain subqueries are considered expensive.
        @todo: accumulate the cost of subqueries.
      */
      return true;
    }

    examined_rows+= cur_join->get_examined_rows();
  }

  // here we are sure that subquery is optimized so thd is set
  return !all_are_simple &&
    (examined_rows > thd->variables.expensive_subquery_limit);
}