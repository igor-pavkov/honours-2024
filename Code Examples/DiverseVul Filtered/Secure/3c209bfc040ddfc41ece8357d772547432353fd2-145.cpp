void subselect_uniquesubquery_engine::print(String *str,
                                            enum_query_type query_type)
{
  char *table_name= tab->table->s->table_name.str;
  str->append(STRING_WITH_LEN("<primary_index_lookup>("));
  tab->ref.items[0]->print(str, query_type);
  str->append(STRING_WITH_LEN(" in "));
  if (tab->table->s->table_category == TABLE_CATEGORY_TEMPORARY)
  {
    /*
      Temporary tables' names change across runs, so they can't be used for
      EXPLAIN EXTENDED.
    */
    str->append(STRING_WITH_LEN("<temporary table>"));
  }
  else
    str->append(table_name, tab->table->s->table_name.length);
  KEY *key_info= tab->table->key_info+ tab->ref.key;
  str->append(STRING_WITH_LEN(" on "));
  str->append(key_info->name);
  if (cond)
  {
    str->append(STRING_WITH_LEN(" where "));
    cond->print(str, query_type);
  }
  str->append(')');
}