void Item_allany_subselect::print(String *str, enum_query_type query_type)
{
  if (test_strategy(SUBS_IN_TO_EXISTS))
    str->append(STRING_WITH_LEN("<exists>"));
  else
  {
    left_expr->print(str, query_type);
    str->append(' ');
    str->append(func->symbol(all));
    str->append(all ? " all " : " any ", 5);
  }
  Item_subselect::print(str, query_type);
}