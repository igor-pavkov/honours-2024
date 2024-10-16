longlong Item_singlerow_subselect::val_int()
{
  DBUG_ASSERT(fixed == 1);
  if (forced_const)
  {
    longlong val= value->val_int();
    null_value= value->null_value;
    return val;
  }
  if (!exec() && !value->null_value)
  {
    null_value= FALSE;
    return value->val_int();
  }
  else
  {
    reset();
    DBUG_ASSERT(null_value);
    return 0;
  }
}