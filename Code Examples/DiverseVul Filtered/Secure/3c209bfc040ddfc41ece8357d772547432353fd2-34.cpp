void Item_exists_subselect::print(String *str, enum_query_type query_type)
{
  str->append(STRING_WITH_LEN("exists"));
  Item_subselect::print(str, query_type);
}