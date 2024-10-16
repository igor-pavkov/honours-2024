subselect_single_select_engine::
subselect_single_select_engine(st_select_lex *select,
			       select_result_interceptor *result_arg,
			       Item_subselect *item_arg)
  :subselect_engine(item_arg, result_arg),
   prepared(0), executed(0),
   select_lex(select), join(0)
{
  select_lex->master_unit()->item= item_arg;
}