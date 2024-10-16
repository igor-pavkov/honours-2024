bool subselect_single_select_engine::no_tables()
{
  return(select_lex->table_list.elements == 0);
}