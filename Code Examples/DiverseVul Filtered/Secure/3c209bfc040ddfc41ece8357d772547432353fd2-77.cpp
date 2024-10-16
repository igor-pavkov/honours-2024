double get_fanout_with_deps(JOIN *join, table_map tset)
{
  /* Handle the case of "Impossible WHERE" */
  if (join->table_count == 0)
    return 0.0;

  /* First, recursively get all tables we depend on */
  table_map deps_to_check= tset;
  table_map checked_deps= 0;
  table_map further_deps;
  do
  {
    further_deps= 0;
    Table_map_iterator tm_it(deps_to_check);
    int tableno;
    while ((tableno = tm_it.next_bit()) != Table_map_iterator::BITMAP_END)
    {
      /* get tableno's dependency tables that are not in needed_set */
      further_deps |= join->map2table[tableno]->ref.depend_map & ~checked_deps;
    }

    checked_deps |= deps_to_check;
    deps_to_check= further_deps;
  } while (further_deps != 0);

  
  /* Now, walk the join order and calculate the fanout */
  double fanout= 1;
  for (JOIN_TAB *tab= first_top_level_tab(join, WITHOUT_CONST_TABLES); tab;
       tab= next_top_level_tab(join, tab))
  {
    /* 
      Ignore SJM nests. They have tab->table==NULL. There is no point to walk
      inside them, because GROUP BY clause cannot refer to tables from within
      subquery.
    */
    if (!tab->is_sjm_nest() && (tab->table->map & checked_deps) && 
        !tab->emb_sj_nest && 
        tab->records_read != 0)
    {
      fanout *= tab->records_read;
    }
  } 
  return fanout;
}