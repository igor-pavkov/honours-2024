Item_allany_subselect::Item_allany_subselect(THD *thd, Item * left_exp,
                                             chooser_compare_func_creator fc,
					     st_select_lex *select_lex,
					     bool all_arg):
  Item_in_subselect(thd), func_creator(fc), all(all_arg)
{
  DBUG_ENTER("Item_allany_subselect::Item_allany_subselect");
  left_expr_orig= left_expr= left_exp;
  /* prepare to possible disassembling the item in convert_subq_to_sj() */
  if (left_exp->type() == Item::ROW_ITEM)
    left_expr_orig= new (thd->mem_root)
      Item_row(thd, static_cast<Item_row*>(left_exp));
  func= func_creator(all_arg);
  init(select_lex, new (thd->mem_root) select_exists_subselect(thd, this));
  max_columns= 1;
  abort_on_null= 0;
  reset();
  //if test_limit will fail then error will be reported to client
  test_limit(select_lex->master_unit());
  DBUG_VOID_RETURN;
}