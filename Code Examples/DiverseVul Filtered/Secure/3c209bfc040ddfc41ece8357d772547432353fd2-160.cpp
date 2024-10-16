Item_singlerow_subselect::Item_singlerow_subselect(THD *thd, st_select_lex *select_lex):
  Item_subselect(thd), value(0)
{
  DBUG_ENTER("Item_singlerow_subselect::Item_singlerow_subselect");
  init(select_lex, new (thd->mem_root) select_singlerow_subselect(thd, this));
  maybe_null= 1;
  max_columns= UINT_MAX;
  DBUG_VOID_RETURN;
}