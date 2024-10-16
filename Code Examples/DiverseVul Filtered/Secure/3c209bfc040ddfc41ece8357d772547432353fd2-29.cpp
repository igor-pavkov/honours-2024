String *Item_singlerow_subselect::val_str(String *str)
{
  DBUG_ASSERT(fixed == 1);
  if (forced_const)
  {
    String *res= value->val_str(str);
    null_value= value->null_value;
    return res;
  }
  if (!exec() && !value->null_value)
  {
    null_value= FALSE;
    return value->val_str(str);
  }
  else
  {
    reset();
    DBUG_ASSERT(null_value);
    return 0;
  }
}