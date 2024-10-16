double get_post_group_estimate(JOIN* join, double join_op_rows)
{
  table_map tables_in_group_list= table_map(0);

  /* Find out which tables are used in GROUP BY list */
  for (ORDER *order= join->group_list_for_estimates; order; order= order->next)
  {
    Item *item= order->item[0];
    table_map item_used_tables= item->used_tables();
    if (item_used_tables & RAND_TABLE_BIT)
    {
      /* Each join output record will be in its own group */
      return join_op_rows;
    }
    tables_in_group_list|= item_used_tables;
  }
  tables_in_group_list &= ~PSEUDO_TABLE_BITS;

  /*
    Use join fanouts to calculate the max. number of records in the group-list
  */
  double fanout_rows[MAX_KEY];
  bzero(&fanout_rows, sizeof(fanout_rows));
  double out_rows;
  
  out_rows= get_fanout_with_deps(join, tables_in_group_list);

#if 0
  /* The following will be needed when making use of index stats: */
  /* 
    Also generate max. number of records for each of the tables mentioned 
    in the group-list. We'll use that a baseline number that we'll try to 
    reduce by using
     - #table-records 
     - index statistics.
  */
  Table_map_iterator tm_it(tables_in_group_list);
  int tableno;
  while ((tableno = tm_it.next_bit()) != Table_map_iterator::BITMAP_END)
  {
    fanout_rows[tableno]= get_fanout_with_deps(join, table_map(1) << tableno);
  }
  
  /*
    Try to bring down estimates using index statistics.
  */
  //check_out_index_stats(join);
#endif

  return out_rows;
}