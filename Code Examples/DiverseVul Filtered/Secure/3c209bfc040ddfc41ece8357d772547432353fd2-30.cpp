my_decimal *Item_singlerow_subselect::val_decimal(my_decimal *decimal_value)
{
  DBUG_ASSERT(fixed == 1);
  if (forced_const)
  {
    my_decimal *val= value->val_decimal(decimal_value);
    null_value= value->null_value;
    return val;
  }
  if (!exec() && !value->null_value)
  {
    null_value= FALSE;
    return value->val_decimal(decimal_value);
  }
  else
  {
    reset();
    DBUG_ASSERT(null_value);
    return 0;
  }
}