void check_out_index_stats(JOIN *join)
{
  ORDER *order;
  uint n_order_items;

  /*
    First, collect the keys that we can use in each table.
    We can use a key if 
    - all tables refer to it.
  */
  key_map key_start_use[MAX_TABLES];
  key_map key_infix_use[MAX_TABLES];
  table_map key_used=0;
  table_map non_key_used= 0;
  
  bzero(&key_start_use, sizeof(key_start_use)); //psergey-todo: safe initialization!
  bzero(&key_infix_use, sizeof(key_infix_use));
  
  for (order= join->group_list; order; order= order->next)
  {
    Item *item= order->item[0];

    if (item->real_type() == Item::FIELD_ITEM)
    {
      if (item->used_tables() & OUTER_REF_TABLE_BIT)
        continue; /* outside references are like constants for us */

      Field *field= ((Item_field*)item->real_item())->field;
      uint table_no= field->table->tablenr;
      if (!(non_key_used && table_map(1) << table_no) && 
          !field->part_of_key.is_clear_all())
      {
        key_map infix_map= field->part_of_key;
        infix_map.subtract(field->key_start);
        key_start_use[table_no].merge(field->key_start);
        key_infix_use[table_no].merge(infix_map);
        key_used |= table_no;
      }
      continue;
    }
    /* 
      Note: the below will cause clauses like GROUP BY YEAR(date) not to be
      handled. 
    */
    non_key_used |= item->used_tables();
  }
  
  Table_map_iterator tm_it(key_used & ~non_key_used);
  int tableno;
  while ((tableno = tm_it.next_bit()) != Table_map_iterator::BITMAP_END)
  {
    key_map::iterator key_it(key_start_use);
    int keyno;
    while ((keyno = tm_it.next_bit()) != key_map::iterator::BITMAP_END)
    {
      for (order= join->group_list; order; order= order->next)
      {
        Item *item= order->item[0];
        if (item->used_tables() & (table_map(1) << tableno))
        {
          DBUG_ASSERT(item->real_type() == Item::FIELD_ITEM);
        }
      }
      /*
      if (continuation)
      {
        walk through list and find which key parts are occupied;
        // note that the above can't be made any faster.
      }
      else
        use rec_per_key[0];
      
      find out the cardinality.
      check if cardinality decreases if we use it;
      */
    }
  }
}