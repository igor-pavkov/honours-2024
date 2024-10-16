Item_in_subselect::single_value_transformer(JOIN *join)
{
  SELECT_LEX *select_lex= join->select_lex;
  DBUG_ENTER("Item_in_subselect::single_value_transformer");
  DBUG_ASSERT(thd == join->thd);

  /*
    Check that the right part of the subselect contains no more than one
    column. E.g. in SELECT 1 IN (SELECT * ..) the right part is (SELECT * ...)
  */
  // psergey: duplicated_subselect_card_check
  if (select_lex->item_list.elements > 1)
  {
    my_error(ER_OPERAND_COLUMNS, MYF(0), 1);
    DBUG_RETURN(true);
  }

  Item* join_having= join->having ? join->having : join->tmp_having;
  if (!(join_having || select_lex->with_sum_func ||
        select_lex->group_list.elements) &&
      select_lex->table_list.elements == 0 && !join->conds &&
      !select_lex->master_unit()->is_union())
  {
    Item *where_item= (Item*) select_lex->item_list.head();
    /*
      it is single select without tables => possible optimization
      remove the dependence mark since the item is moved to upper
      select and is not outer anymore.
    */
    where_item->walk(&Item::remove_dependence_processor, 0,
                     select_lex->outer_select());
    /*
      fix_field of substitution item will be done in time of
      substituting.
      Note that real_item() should be used instead of
      original left expression because left_expr can be
      runtime created Ref item which is deleted at the end
      of the statement. Thus one of 'substitution' arguments
      can be broken in case of PS.
    */ 
    substitution= func->create(thd, left_expr, where_item);
    have_to_be_excluded= 1;
    if (thd->lex->describe)
    {
      char warn_buff[MYSQL_ERRMSG_SIZE];
      sprintf(warn_buff, ER_THD(thd, ER_SELECT_REDUCED),
              select_lex->select_number);
      push_warning(thd, Sql_condition::WARN_LEVEL_NOTE,
                   ER_SELECT_REDUCED, warn_buff);
    }
    DBUG_RETURN(false);
  }

  /*
    Wrap the current IN predicate in an Item_in_optimizer. The actual
    substitution in the Item tree takes place in Item_subselect::fix_fields.
  */
  if (!substitution)
  {
    /* We're invoked for the 1st (or the only) SELECT in the subquery UNION */
    substitution= optimizer;

    SELECT_LEX *current= thd->lex->current_select;

    thd->lex->current_select= current->return_after_parsing();
    if (!optimizer || optimizer->fix_left(thd))
    {
      thd->lex->current_select= current;
      DBUG_RETURN(true);
    }
    thd->lex->current_select= current;

    /* We will refer to upper level cache array => we have to save it for SP */
    optimizer->keep_top_level_cache();

    /*
      As far as  Item_in_optimizer does not substitute itself on fix_fields
      we can use same item for all selects.
    */
    expr= new (thd->mem_root) Item_direct_ref(thd, &select_lex->context,
                              (Item**)optimizer->get_cache(),
			      (char *)"<no matter>",
			      (char *)in_left_expr_name);
  }

  DBUG_RETURN(false);
}