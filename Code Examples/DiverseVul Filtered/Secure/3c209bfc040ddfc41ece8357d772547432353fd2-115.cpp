Item_exists_subselect::Item_exists_subselect(THD *thd,
                                             st_select_lex *select_lex):
  Item_subselect(thd), upper_not(NULL), abort_on_null(0),
  emb_on_expr_nest(NULL), optimizer(0), exists_transformed(0)
{
  DBUG_ENTER("Item_exists_subselect::Item_exists_subselect");
  init(select_lex, new (thd->mem_root) select_exists_subselect(thd, this));
  max_columns= UINT_MAX;
  null_value= FALSE; //can't be NULL
  maybe_null= 0; //can't be NULL
  value= 0;
  DBUG_VOID_RETURN;
}