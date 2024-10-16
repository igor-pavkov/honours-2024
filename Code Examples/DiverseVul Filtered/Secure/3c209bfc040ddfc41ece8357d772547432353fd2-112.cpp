table_map subselect_union_engine::upper_select_const_tables()
{
  return calc_const_tables(unit->outer_select()->leaf_tables);
}