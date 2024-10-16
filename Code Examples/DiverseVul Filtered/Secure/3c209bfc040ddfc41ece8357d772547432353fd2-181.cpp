Item_in_subselect::create_single_in_to_exists_cond(JOIN *join,
                                                   Item **where_item,
                                                   Item **having_item)
{
  SELECT_LEX *select_lex= join->select_lex;
  DBUG_ASSERT(thd == join->thd);
  /*
    The non-transformed HAVING clause of 'join' may be stored in two ways
    during JOIN::optimize: this->tmp_having= this->having; this->having= 0;
  */
  Item* join_having= join->having ? join->having : join->tmp_having;
  DBUG_ENTER("Item_in_subselect::create_single_in_to_exists_cond");

  *where_item= NULL;
  *having_item= NULL;

  if (join_having || select_lex->with_sum_func ||
      select_lex->group_list.elements)
  {
    Item *item= func->create(thd, expr,
                             new (thd->mem_root) Item_ref_null_helper(
                                                      thd,
                                                      &select_lex->context,
                                                      this,
                                                      &select_lex->
                                                      ref_pointer_array[0],  
                                                      (char *)"<ref>",
                                                      this->full_name()));
    if (!abort_on_null && left_expr->maybe_null)
    {
      /* 
        We can encounter "NULL IN (SELECT ...)". Wrap the added condition
        within a trig_cond.
      */
      disable_cond_guard_for_const_null_left_expr(0);
      item= new (thd->mem_root) Item_func_trig_cond(thd, item, get_cond_guard(0));
    }

    if (!join_having)
      item->name= (char*) in_having_cond;
    if (fix_having(item, select_lex))
      DBUG_RETURN(true);
    *having_item= item;
  }
  else
  {
    /*
      No need to use real_item for the item, as the ref items that are possible
      in the subquery either belong to views or to the parent select.
      For such case we need to refer to the reference and not to the original
      item.
    */
    Item *item= (Item*) select_lex->item_list.head();

    if (select_lex->table_list.elements ||
        !(select_lex->master_unit()->is_union()))
    {
      Item *having= item;
      Item *orig_item= item;
       
      item= func->create(thd, expr, item);
      if (!abort_on_null && orig_item->maybe_null)
      {
	having= new (thd->mem_root) Item_is_not_null_test(thd, this, having);
        if (left_expr->maybe_null)
        {
          disable_cond_guard_for_const_null_left_expr(0);
          if (!(having= new (thd->mem_root) Item_func_trig_cond(thd, having,
                                                            get_cond_guard(0))))
            DBUG_RETURN(true);
        }
        having->name= (char*) in_having_cond;
        if (fix_having(having, select_lex))
          DBUG_RETURN(true);
        *having_item= having;

	item= new (thd->mem_root) Item_cond_or(thd, item,
                               new (thd->mem_root) Item_func_isnull(thd, orig_item));
      }
      /* 
        If we may encounter NULL IN (SELECT ...) and care whether subquery
        result is NULL or FALSE, wrap condition in a trig_cond.
      */
      if (!abort_on_null && left_expr->maybe_null)
      {
        disable_cond_guard_for_const_null_left_expr(0);
        if (!(item= new (thd->mem_root) Item_func_trig_cond(thd, item,
                                                            get_cond_guard(0))))
          DBUG_RETURN(true);
      }

      /*
        TODO: figure out why the following is done here in 
        single_value_transformer but there is no corresponding action in
        row_value_transformer?
      */
      item->name= (char *) in_additional_cond;
      if (!item->fixed && item->fix_fields(thd, 0))
        DBUG_RETURN(true);
      *where_item= item;
    }
    else
    {
      DBUG_ASSERT(select_lex->master_unit()->is_union());

      Item *new_having=
        func->create(thd, expr,
                     new (thd->mem_root) Item_ref_null_helper(thd,
                                                &select_lex->context,
                                                this,
                                                &select_lex->ref_pointer_array[0],
                                                (char *)"<no matter>",
                                                (char *)"<result>"));
      if (!abort_on_null && left_expr->maybe_null)
      {
        disable_cond_guard_for_const_null_left_expr(0);
        if (!(new_having= new (thd->mem_root) Item_func_trig_cond(thd, new_having,
                                                          get_cond_guard(0))))
          DBUG_RETURN(true);
      }

      new_having->name= (char*) in_having_cond;
      if (fix_having(new_having, select_lex))
        DBUG_RETURN(true);
      *having_item= new_having;
    }
  }

  DBUG_RETURN(false);
}