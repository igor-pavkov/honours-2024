Item_in_subselect::row_value_transformer(JOIN *join)
{
  SELECT_LEX *select_lex= join->select_lex;
  uint cols_num= left_expr->cols();

  DBUG_ENTER("Item_in_subselect::row_value_transformer");
  DBUG_ASSERT(thd == join->thd);

  // psergey: duplicated_subselect_card_check
  if (select_lex->item_list.elements != cols_num)
  {
    my_error(ER_OPERAND_COLUMNS, MYF(0), cols_num);
    DBUG_RETURN(true);
  }

  /*
    Wrap the current IN predicate in an Item_in_optimizer. The actual
    substitution in the Item tree takes place in Item_subselect::fix_fields.
  */
  if (!substitution)
  {
    //first call for this unit
    SELECT_LEX_UNIT *master_unit= select_lex->master_unit();
    substitution= optimizer;

    SELECT_LEX *current= thd->lex->current_select;
    thd->lex->current_select= current->return_after_parsing();
    if (!optimizer || optimizer->fix_left(thd))
    {
      thd->lex->current_select= current;
      DBUG_RETURN(true);
    }

    // we will refer to upper level cache array => we have to save it in PS
    optimizer->keep_top_level_cache();

    thd->lex->current_select= current;
    /*
      The uncacheable property controls a number of actions, e.g. whether to
      save/restore (via init_save_join_tab/restore_tmp) the original JOIN for
      plans with a temp table where the original JOIN was overridden by
      make_simple_join. The UNCACHEABLE_EXPLAIN is ignored by EXPLAIN, thus
      non-correlated subqueries will not appear as such to EXPLAIN.
    */
    master_unit->uncacheable|= UNCACHEABLE_EXPLAIN;
    select_lex->uncacheable|= UNCACHEABLE_EXPLAIN;
  }

  DBUG_RETURN(false);
}