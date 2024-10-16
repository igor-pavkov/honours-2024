table_map subselect_single_select_engine::upper_select_const_tables()
{
  return calc_const_tables(select_lex->outer_select()->leaf_tables);
}