Item_in_subselect::create_row_in_to_exists_cond(JOIN * join,
                                                Item **where_item,
                                                Item **having_item)
{
  SELECT_LEX *select_lex= join->select_lex;
  uint cols_num= left_expr->cols();
  /*
    The non-transformed HAVING clause of 'join' may be stored in two ways
    during JOIN::optimize: this->tmp_having= this->having; this->having= 0;
  */
  Item* join_having= join->having ? join->having : join->tmp_having;
  bool is_having_used= (join_having || select_lex->with_sum_func ||
                        select_lex->group_list.first ||
                        !select_lex->table_list.elements);

  DBUG_ENTER("Item_in_subselect::create_row_in_to_exists_cond");
  DBUG_ASSERT(thd == join->thd);

  *where_item= NULL;
  *having_item= NULL;

  if (is_having_used)
  {
    /* TODO: say here explicitly if the order of AND parts matters or not. */
    Item *item_having_part2= 0;
    for (uint i= 0; i < cols_num; i++)
    {
      DBUG_ASSERT((left_expr->fixed &&

                  select_lex->ref_pointer_array[i]->fixed) ||
                  (select_lex->ref_pointer_array[i]->type() == REF_ITEM &&
                   ((Item_ref*)(select_lex->ref_pointer_array[i]))->ref_type() ==
                    Item_ref::OUTER_REF));
      if (select_lex->ref_pointer_array[i]->
          check_cols(left_expr->element_index(i)->cols()))
        DBUG_RETURN(true);
      Item *item_eq=
        new (thd->mem_root)
        Item_func_eq(thd, new (thd->mem_root)
                     Item_direct_ref(thd, &select_lex->context,
                                     (*optimizer->get_cache())->
                                     addr(i),
                                     (char *)"<no matter>",
                                     (char *)in_left_expr_name),
                     new (thd->mem_root)
                     Item_ref(thd, &select_lex->context,
                              &select_lex->ref_pointer_array[i],
                              (char *)"<no matter>",
                              (char *)"<list ref>"));
      Item *item_isnull=
        new (thd->mem_root)
        Item_func_isnull(thd,
                         new (thd->mem_root)
                         Item_ref(thd, &select_lex->context,
                                  &select_lex->ref_pointer_array[i],
                                  (char *)"<no matter>",
                                  (char *)"<list ref>"));
      Item *col_item= new (thd->mem_root)
        Item_cond_or(thd, item_eq, item_isnull);
      if (!abort_on_null && left_expr->element_index(i)->maybe_null &&
          get_cond_guard(i))
      {
        disable_cond_guard_for_const_null_left_expr(i);
        if (!(col_item= new (thd->mem_root)
              Item_func_trig_cond(thd, col_item, get_cond_guard(i))))
          DBUG_RETURN(true);
      }
      *having_item= and_items(thd, *having_item, col_item);

      Item *item_nnull_test= 
         new (thd->mem_root)
        Item_is_not_null_test(thd, this,
                              new (thd->mem_root)
                              Item_ref(thd, &select_lex->context,
                                       &select_lex->
                                       ref_pointer_array[i],
                                       (char *)"<no matter>",
                                       (char *)"<list ref>"));
      if (!abort_on_null && left_expr->element_index(i)->maybe_null &&
          get_cond_guard(i) )
      {
        disable_cond_guard_for_const_null_left_expr(i);
        if (!(item_nnull_test= 
              new (thd->mem_root)
              Item_func_trig_cond(thd, item_nnull_test, get_cond_guard(i))))
          DBUG_RETURN(true);
      }
      item_having_part2= and_items(thd, item_having_part2, item_nnull_test);
      item_having_part2->top_level_item();
    }
    *having_item= and_items(thd, *having_item, item_having_part2);
  }
  else
  {
    for (uint i= 0; i < cols_num; i++)
    {
      Item *item, *item_isnull;
      DBUG_ASSERT((left_expr->fixed &&
                  select_lex->ref_pointer_array[i]->fixed) ||
                  (select_lex->ref_pointer_array[i]->type() == REF_ITEM &&
                   ((Item_ref*)(select_lex->ref_pointer_array[i]))->ref_type() ==
                    Item_ref::OUTER_REF));
      if (select_lex->ref_pointer_array[i]->
          check_cols(left_expr->element_index(i)->cols()))
        DBUG_RETURN(true);
      item= new (thd->mem_root)
        Item_func_eq(thd,
                     new (thd->mem_root)
                     Item_direct_ref(thd, &select_lex->context,
                                     (*optimizer->get_cache())->
                                     addr(i),
                                     (char *)"<no matter>",
                                     (char *)in_left_expr_name),
                     new (thd->mem_root)
                     Item_direct_ref(thd, &select_lex->context,
                                     &select_lex->
                                     ref_pointer_array[i],
                                     (char *)"<no matter>",
                                     (char *)"<list ref>"));
      if (!abort_on_null && select_lex->ref_pointer_array[i]->maybe_null)
      {
        Item *having_col_item=
          new (thd->mem_root)
          Item_is_not_null_test(thd, this,
                                new (thd->mem_root)
                                Item_ref(thd, &select_lex->context, 
                                         &select_lex->ref_pointer_array[i],
                                         (char *)"<no matter>",
                                         (char *)"<list ref>"));
        
        
        item_isnull= new (thd->mem_root)
          Item_func_isnull(thd,
                           new (thd->mem_root)
                           Item_direct_ref(thd, &select_lex->context,
                                           &select_lex->
                                           ref_pointer_array[i],
                                           (char *)"<no matter>",
                                           (char *)"<list ref>"));
        item= new (thd->mem_root) Item_cond_or(thd, item, item_isnull);
        if (left_expr->element_index(i)->maybe_null && get_cond_guard(i))
        {
          disable_cond_guard_for_const_null_left_expr(i);
          if (!(item= new (thd->mem_root)
                Item_func_trig_cond(thd, item, get_cond_guard(i))))
            DBUG_RETURN(true);
          if (!(having_col_item= new (thd->mem_root)
                Item_func_trig_cond(thd, having_col_item, get_cond_guard(i))))
            DBUG_RETURN(true);
        }
        *having_item= and_items(thd, *having_item, having_col_item);
      }
      if (!abort_on_null && left_expr->element_index(i)->maybe_null &&
          get_cond_guard(i))
      {
        if (!(item= new (thd->mem_root)
              Item_func_trig_cond(thd, item, get_cond_guard(i))))
          DBUG_RETURN(true);
      }
      *where_item= and_items(thd, *where_item, item);
    }
  }

  if (*where_item)
  {
    if (!(*where_item)->fixed && (*where_item)->fix_fields(thd, 0))
      DBUG_RETURN(true);
    (*where_item)->top_level_item();
  }

  if (*having_item)
  {
    if (!join_having)
      (*having_item)->name= (char*) in_having_cond;
    if (fix_having(*having_item, select_lex))
      DBUG_RETURN(true);
    (*having_item)->top_level_item();
  }

  DBUG_RETURN(false);
}