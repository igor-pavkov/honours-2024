int walk_items_for_table_list(Item_processor processor,
                              bool walk_subquery, void *argument,
                              List<TABLE_LIST>& join_list)
{
  List_iterator<TABLE_LIST> li(join_list);
  int res;
  while (TABLE_LIST *table= li++)
  {
    if (table->on_expr)
    {
      if ((res= table->on_expr->walk(processor, walk_subquery, argument)))
        return res;
    }
    if (table->nested_join)
    {
      if ((res= walk_items_for_table_list(processor, walk_subquery, argument,
                                          table->nested_join->join_list)))
        return res;
    }
  }
  return 0;
}