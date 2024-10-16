void subselect_indexsubquery_engine::print(String *str,
                                           enum_query_type query_type)
{
  str->append(STRING_WITH_LEN("<index_lookup>("));
  tab->ref.items[0]->print(str, query_type);
  str->append(STRING_WITH_LEN(" in "));
  str->append(tab->table->s->table_name.str, tab->table->s->table_name.length);
  KEY *key_info= tab->table->key_info+ tab->ref.key;
  str->append(STRING_WITH_LEN(" on "));
  str->append(key_info->name);
  if (check_null)
    str->append(STRING_WITH_LEN(" checking NULL"));
  if (cond)
  {
    str->append(STRING_WITH_LEN(" where "));
    cond->print(str, query_type);
  }
  if (having)
  {
    str->append(STRING_WITH_LEN(" having "));
    having->print(str, query_type);
  }
  str->append(')');
}