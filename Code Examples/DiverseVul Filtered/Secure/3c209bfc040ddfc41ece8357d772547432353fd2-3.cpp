void subselect_hash_sj_engine::print(String *str, enum_query_type query_type)
{
  str->append(STRING_WITH_LEN(" <materialize> ("));
  materialize_engine->print(str, query_type);
  str->append(STRING_WITH_LEN(" ), "));

  if (lookup_engine)
    lookup_engine->print(str, query_type);
  else
    str->append(STRING_WITH_LEN(
           "<engine selected at execution time>"
         ));
}