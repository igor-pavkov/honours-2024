void Item_in_subselect::print(String *str, enum_query_type query_type)
{
  if (test_strategy(SUBS_IN_TO_EXISTS))
    str->append(STRING_WITH_LEN("<exists>"));
  else
  {
    left_expr->print_parenthesised(str, query_type, precedence());
    str->append(STRING_WITH_LEN(" in "));
  }
  Item_subselect::print(str, query_type);
}