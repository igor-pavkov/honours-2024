double Item_singlerow_subselect::val_real()
{
  DBUG_ASSERT(fixed == 1);
  if (forced_const)
    return value->val_real();
  if (!exec() && !value->null_value)
  {
    null_value= FALSE;
    return value->val_real();
  }
  else
  {
    reset();
    return 0;
  }
}