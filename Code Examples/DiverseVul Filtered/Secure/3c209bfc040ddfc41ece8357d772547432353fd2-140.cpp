void subselect_union_engine::cleanup()
{
  DBUG_ENTER("subselect_union_engine::cleanup");
  unit->reinit_exec_mechanism();
  result->cleanup();
  unit->uncacheable&= ~UNCACHEABLE_DEPENDENT_INJECTED;
  for (SELECT_LEX *sl= unit->first_select(); sl; sl= sl->next_select())
    sl->uncacheable&= ~UNCACHEABLE_DEPENDENT_INJECTED;
  DBUG_VOID_RETURN;
}