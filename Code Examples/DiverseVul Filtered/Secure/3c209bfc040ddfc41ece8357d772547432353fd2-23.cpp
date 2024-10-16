void subselect_single_select_engine::print(String *str,
                                           enum_query_type query_type)
{
  With_clause* with_clause= select_lex->get_with_clause();
  if (with_clause)
    with_clause->print(str, query_type);
  select_lex->print(get_thd(), str, query_type);
}