bool Item_subselect::walk(Item_processor processor, bool walk_subquery,
                          void *argument)
{
  if (!(unit->uncacheable & ~UNCACHEABLE_DEPENDENT) && engine->is_executed() &&
      !unit->describe)
  {
    /*
      The subquery has already been executed (for real, it wasn't EXPLAIN's
      fake execution) so it should not matter what it has inside.
      
      The actual reason for not walking inside is that parts of the subquery
      (e.g. JTBM join nests and their IN-equality conditions may have been 
       invalidated by irreversible cleanups (those happen after an uncorrelated 
       subquery has been executed).
    */
    return (this->*processor)(argument);
  }

  if (walk_subquery)
  {
    for (SELECT_LEX *lex= unit->first_select(); lex; lex= lex->next_select())
    {
      List_iterator<Item> li(lex->item_list);
      Item *item;
      ORDER *order;

      if (lex->where && (lex->where)->walk(processor, walk_subquery, argument))
        return 1;
      if (lex->having && (lex->having)->walk(processor, walk_subquery,
                                             argument))
        return 1;

     if (walk_items_for_table_list(processor, walk_subquery, argument,
                                       *lex->join_list))
        return 1;

      while ((item=li++))
      {
        if (item->walk(processor, walk_subquery, argument))
          return 1;
      }
      for (order= lex->order_list.first ; order; order= order->next)
      {
        if ((*order->item)->walk(processor, walk_subquery, argument))
          return 1;
      }
      for (order= lex->group_list.first ; order; order= order->next)
      {
        if ((*order->item)->walk(processor, walk_subquery, argument))
          return 1;
      }
    }
  }
  return (this->*processor)(argument);
}