subselect_union_engine::subselect_union_engine(st_select_lex_unit *u,
					       select_result_interceptor *result_arg,
					       Item_subselect *item_arg)
  :subselect_engine(item_arg, result_arg)
{
  unit= u;
  unit->item= item_arg;
}