void subselect_single_select_engine::cleanup()
{
  DBUG_ENTER("subselect_single_select_engine::cleanup");
  prepared= executed= 0;
  join= 0;
  result->cleanup();
  select_lex->uncacheable&= ~UNCACHEABLE_DEPENDENT_INJECTED;
  DBUG_VOID_RETURN;
}