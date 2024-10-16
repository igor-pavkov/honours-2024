bool Item_in_subselect::val_bool()
{
  DBUG_ASSERT(fixed == 1);
  if (forced_const)
    return value;
  DBUG_ASSERT((engine->uncacheable() & ~UNCACHEABLE_EXPLAIN) ||
              ! engine->is_executed() || with_recursive_reference);
  null_value= was_null= FALSE;
  if (exec())
  {
    reset();
    return 0;
  }
  if (was_null && !value)
    null_value= TRUE;
  return value;
}