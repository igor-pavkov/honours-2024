bool Item_in_subselect::create_in_to_exists_cond(JOIN *join_arg)
{
  bool res;

  DBUG_ASSERT(engine->engine_type() == subselect_engine::SINGLE_SELECT_ENGINE ||
              engine->engine_type() == subselect_engine::UNION_ENGINE);
  /*
    TODO: the call to init_cond_guards allocates and initializes an
    array of booleans that may not be used later because we may choose
    materialization.
    The two calls below to create_XYZ_cond depend on this boolean array.
    If the dependency is removed, the call can be moved to a later phase.
  */
  init_cond_guards();
  if (left_expr->cols() == 1)
    res= create_single_in_to_exists_cond(join_arg,
                                         &(join_arg->in_to_exists_where),
                                         &(join_arg->in_to_exists_having));
  else
    res= create_row_in_to_exists_cond(join_arg,
                                      &(join_arg->in_to_exists_where),
                                      &(join_arg->in_to_exists_having));

  /*
    The IN=>EXISTS transformation makes non-correlated subqueries correlated.
  */
  if (!left_expr->const_item() || left_expr->is_expensive())
  {
    join_arg->select_lex->uncacheable|= UNCACHEABLE_DEPENDENT_INJECTED;
    join_arg->select_lex->master_unit()->uncacheable|= 
                                         UNCACHEABLE_DEPENDENT_INJECTED;
  }
  /*
    The uncacheable property controls a number of actions, e.g. whether to
    save/restore (via init_save_join_tab/restore_tmp) the original JOIN for
    plans with a temp table where the original JOIN was overridden by
    make_simple_join. The UNCACHEABLE_EXPLAIN is ignored by EXPLAIN, thus
    non-correlated subqueries will not appear as such to EXPLAIN.
  */
  join_arg->select_lex->master_unit()->uncacheable|= UNCACHEABLE_EXPLAIN;
  join_arg->select_lex->uncacheable|= UNCACHEABLE_EXPLAIN;
  return (res);
}