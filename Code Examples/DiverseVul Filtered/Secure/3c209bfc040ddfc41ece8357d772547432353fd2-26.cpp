bool Item_in_subselect::fix_fields(THD *thd_arg, Item **ref)
{
  uint outer_cols_num;
  List<Item> *inner_cols;
  char const *save_where= thd_arg->where;
  DBUG_ENTER("Item_in_subselect::fix_fields");

  thd= thd_arg;
  DBUG_ASSERT(unit->thd == thd);

  if (test_strategy(SUBS_SEMI_JOIN))
    DBUG_RETURN( !( (*ref)= new (thd->mem_root) Item_int(thd, 1)) );

  thd->where= "IN/ALL/ANY subquery";
  /*
    Check if the outer and inner IN operands match in those cases when we
    will not perform IN=>EXISTS transformation. Currently this is when we
    use subquery materialization.

    The condition below is true when this method was called recursively from
    inside JOIN::prepare for the JOIN object created by the call chain
    Item_subselect::fix_fields -> subselect_single_select_engine::prepare,
    which creates a JOIN object for the subquery and calls JOIN::prepare for
    the JOIN of the subquery.
    Notice that in some cases, this doesn't happen, and the check_cols()
    test for each Item happens later in
    Item_in_subselect::row_value_in_to_exists_transformer.
    The reason for this mess is that our JOIN::prepare phase works top-down
    instead of bottom-up, so we first do name resoluton and semantic checks
    for the outer selects, then for the inner.
  */
  if (engine &&
      engine->engine_type() == subselect_engine::SINGLE_SELECT_ENGINE &&
      ((subselect_single_select_engine*)engine)->join)
  {
    outer_cols_num= left_expr->cols();

    if (unit->is_union())
      inner_cols= &(unit->types);
    else
      inner_cols= &(unit->first_select()->item_list);
    if (outer_cols_num != inner_cols->elements)
    {
      my_error(ER_OPERAND_COLUMNS, MYF(0), outer_cols_num);
      goto err;
    }
    if (outer_cols_num > 1)
    {
      List_iterator<Item> inner_col_it(*inner_cols);
      Item *inner_col;
      for (uint i= 0; i < outer_cols_num; i++)
      {
        inner_col= inner_col_it++;
        if (inner_col->check_cols(left_expr->element_index(i)->cols()))
          goto err;
      }
    }
  }

  if (left_expr && !left_expr->fixed &&
      left_expr->fix_fields(thd_arg, &left_expr))
    goto err;
  else
  if (Item_subselect::fix_fields(thd_arg, ref))
    goto err;
  fixed= TRUE;
  thd->where= save_where;
  DBUG_RETURN(FALSE);

err:
  thd->where= save_where;
  DBUG_RETURN(TRUE);
}