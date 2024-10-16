int Item_in_subselect::optimize(double *out_rows, double *cost)
{
  int res;
  DBUG_ENTER("Item_in_subselect::optimize");
  DBUG_ASSERT(fixed);
  SELECT_LEX *save_select= thd->lex->current_select;
  JOIN *join= unit->first_select()->join;

  thd->lex->current_select= join->select_lex;
  if ((res= join->optimize()))
    DBUG_RETURN(res);

  /* Calculate #rows and cost of join execution */
  join->get_partial_cost_and_fanout(join->table_count - join->const_tables, 
                                    table_map(-1),
                                    cost, out_rows);

  /*
    Adjust join output cardinality. There can be these cases:
    - Have no GROUP BY and no aggregate funcs: we won't get into this 
      function because such join will be processed as a merged semi-join 
      (TODO: does it really mean we don't need to handle such cases here at 
       all? put ASSERT)
    - Have no GROUP BY but have aggregate funcs: output is 1 record.
    - Have GROUP BY and have (or not) aggregate funcs:  need to adjust output 
      cardinality.
  */
  thd->lex->current_select= save_select;
  if (!join->group_list && !join->group_optimized_away &&
      join->tmp_table_param.sum_func_count)
  {
    DBUG_PRINT("info",("Materialized join will have only 1 row (it has "
                       "aggregates but no GROUP BY"));
    *out_rows= 1;
  }
  
  /* Now with grouping */
  if (join->group_list_for_estimates)
  {
    DBUG_PRINT("info",("Materialized join has grouping, trying to estimate it"));
    double output_rows= get_post_group_estimate(join, *out_rows);
    DBUG_PRINT("info",("Got value of %g", output_rows));
    *out_rows= output_rows;
  }

  DBUG_RETURN(res);

}