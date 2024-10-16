bool Item_in_subselect::inject_in_to_exists_cond(JOIN *join_arg)
{
  SELECT_LEX *select_lex= join_arg->select_lex;
  Item *where_item= join_arg->in_to_exists_where;
  Item *having_item= join_arg->in_to_exists_having;

  DBUG_ENTER("Item_in_subselect::inject_in_to_exists_cond");
  DBUG_ASSERT(thd == join_arg->thd);

  if (select_lex->min_max_opt_list.elements)
  {
    /*
      MIN/MAX optimizations have been applied to Item_sum objects
      of the subquery this subquery predicate in opt_sum_query().
      Injection of new condition invalidates this optimizations.
      Thus those optimizations must be rolled back.
    */
    List_iterator_fast<Item_sum> it(select_lex->min_max_opt_list);
    Item_sum *item;
    while ((item= it++))
    {
      item->clear();
      item->reset_forced_const();
    }
    if (where_item)
      where_item->update_used_tables();
    if (having_item)
      having_item->update_used_tables();
  }

  if (where_item)
  {
    List<Item> *and_args= NULL;
    /*
      If the top-level Item of the WHERE clause is an AND, detach the multiple
      equality list that was attached to the end of the AND argument list by
      build_equal_items_for_cond(). The multiple equalities must be detached
      because fix_fields merges lower level AND arguments into the upper AND.
      As a result, the arguments from lower-level ANDs are concatenated after
      the multiple equalities. When the multiple equality list is treated as
      such, it turns out that it contains non-Item_equal object which is wrong.
    */
    if (join_arg->conds && join_arg->conds->type() == Item::COND_ITEM &&
        ((Item_cond*) join_arg->conds)->functype() == Item_func::COND_AND_FUNC)
    {
      and_args= ((Item_cond*) join_arg->conds)->argument_list();
      if (join_arg->cond_equal)
        and_args->disjoin((List<Item> *) &join_arg->cond_equal->current_level);
    }

    where_item= and_items(thd, join_arg->conds, where_item);

    /* This is the fix_fields() call mentioned in the comment above */
    if (!where_item->fixed && where_item->fix_fields(thd, 0))
      DBUG_RETURN(true);
    // TIMOUR TODO: call optimize_cond() for the new where clause
    thd->change_item_tree(&select_lex->where, where_item);
    select_lex->where->top_level_item();
    join_arg->conds= select_lex->where;

    /* Attach back the list of multiple equalities to the new top-level AND. */
    if (and_args && join_arg->cond_equal)
    {
      /*
        The fix_fields() call above may have changed the argument list, so
        fetch it again:
      */
      and_args= ((Item_cond*) join_arg->conds)->argument_list();
      ((Item_cond_and *) (join_arg->conds))->m_cond_equal=
                                             *join_arg->cond_equal;
      and_args->append((List<Item> *)&join_arg->cond_equal->current_level);
    }
  }

  if (having_item)
  {
    Item* join_having= join_arg->having ? join_arg->having:join_arg->tmp_having;
    having_item= and_items(thd, join_having, having_item);
    if (fix_having(having_item, select_lex))
      DBUG_RETURN(true);
    // TIMOUR TODO: call optimize_cond() for the new having clause
    thd->change_item_tree(&select_lex->having, having_item);
    select_lex->having->top_level_item();
    join_arg->having= select_lex->having;
  }
  join_arg->thd->change_item_tree(&unit->global_parameters()->select_limit,
                                  new (thd->mem_root)
                                  Item_int(thd, (int32) 1));
  unit->select_limit_cnt= 1;

  DBUG_RETURN(false);
}