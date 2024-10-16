bool Item_singlerow_subselect::val_bool()
{
  DBUG_ASSERT(fixed == 1);
  if (forced_const)
  {
    bool val= value->val_bool();
    null_value= value->null_value;
    return val;
  }
  if (!exec() && !value->null_value)
  {
    null_value= FALSE;
    return value->val_bool();
  }
  else
  {
    reset();
    DBUG_ASSERT(null_value);
    return 0;
  }
}