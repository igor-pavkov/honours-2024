uint subselect_single_select_engine::cols()
{
  //psergey-sj-backport: the following assert was gone in 6.0:
  //DBUG_ASSERT(select_lex->join != 0); // should be called after fix_fields()
  //return select_lex->join->fields_list.elements;
  return select_lex->item_list.elements;
}