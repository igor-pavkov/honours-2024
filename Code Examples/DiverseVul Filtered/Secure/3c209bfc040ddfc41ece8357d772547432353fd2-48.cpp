void Item_in_subselect::cleanup()
{
  DBUG_ENTER("Item_in_subselect::cleanup");
  if (left_expr_cache)
  {
    left_expr_cache->delete_elements();
    delete left_expr_cache;
    left_expr_cache= NULL;
  }
  /*
    TODO: This breaks the commented assert in add_strategy().
    in_strategy&= ~SUBS_STRATEGY_CHOSEN;
  */
  first_execution= TRUE;
  pushed_cond_guards= NULL;
  Item_subselect::cleanup();
  DBUG_VOID_RETURN;
}