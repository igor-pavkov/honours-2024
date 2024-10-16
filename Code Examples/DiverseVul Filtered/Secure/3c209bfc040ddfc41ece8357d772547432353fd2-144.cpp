longlong Item_in_subselect::val_int()
{
  /*
    As far as Item_in_subselect called only from Item_in_optimizer this
    method should not be used
  */
  DBUG_ASSERT(0);
  DBUG_ASSERT(fixed == 1);
  if (forced_const)
    return value;
  DBUG_ASSERT((engine->uncacheable() & ~UNCACHEABLE_EXPLAIN) ||
              ! engine->is_executed());
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