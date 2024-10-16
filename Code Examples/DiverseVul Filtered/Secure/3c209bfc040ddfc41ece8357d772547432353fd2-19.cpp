table_map subselect_engine::calc_const_tables(List<TABLE_LIST> &list)
{
  table_map map= 0;
  List_iterator<TABLE_LIST> ti(list);
  TABLE_LIST *table;
  //for (; table; table= table->next_leaf)
  while ((table= ti++))
  {
    TABLE *tbl= table->table;
    if (tbl && tbl->const_table)
      map|= tbl->map;
  }
  return map;
}