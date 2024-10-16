Item_in_subselect::select_in_like_transformer(JOIN *join)
{
  Query_arena *arena= 0, backup;
  SELECT_LEX *current= thd->lex->current_select;
  const char *save_where= thd->where;
  bool trans_res= true;
  bool result;

  DBUG_ENTER("Item_in_subselect::select_in_like_transformer");
  DBUG_ASSERT(thd == join->thd);

  /*
    IN/SOME/ALL/ANY subqueries aren't support LIMIT clause. Without it
    ORDER BY clause becomes meaningless thus we drop it here.
  */
  for (SELECT_LEX *sl= current->master_unit()->first_select();
       sl; sl= sl->next_select())
  {
    if (sl->join)
    {
      sl->join->order= 0;
      sl->join->skip_sort_order= 1;
    }
  }

  thd->where= "IN/ALL/ANY subquery";

  /*
    In some optimisation cases we will not need this Item_in_optimizer
    object, but we can't know it here, but here we need address correct
    reference on left expression.

    note: we won't need Item_in_optimizer when handling degenerate cases
    like "... IN (SELECT 1)"
  */
  arena= thd->activate_stmt_arena_if_needed(&backup);
  if (!optimizer)
  {
    optimizer= new (thd->mem_root) Item_in_optimizer(thd, left_expr_orig, this);
    if ((result= !optimizer))
      goto out;
  }

  thd->lex->current_select= current->return_after_parsing();
  result= optimizer->fix_left(thd);
  thd->lex->current_select= current;

  if (changed)
  {
    trans_res= false;
    goto out;
  }


  if (result)
    goto out;

  /*
    Both transformers call fix_fields() only for Items created inside them,
    and all that items do not make permanent changes in current item arena
    which allow to us call them with changed arena (if we do not know nature
    of Item, we have to call fix_fields() for it only with original arena to
    avoid memory leak)
  */
  if (left_expr->cols() == 1)
    trans_res= single_value_transformer(join);
  else
  {
    /* we do not support row operation for ALL/ANY/SOME */
    if (func != &eq_creator)
    {
      if (arena)
        thd->restore_active_arena(arena, &backup);
      my_error(ER_OPERAND_COLUMNS, MYF(0), 1);
      DBUG_RETURN(true);
    }
    trans_res= row_value_transformer(join);
  }
out:
  if (arena)
    thd->restore_active_arena(arena, &backup);
  thd->where= save_where;
  DBUG_RETURN(trans_res);
}