longlong Item_func_lt::val_int()
{
  DBUG_ASSERT(fixed == 1);
  int value= cmp.compare();
  return value < 0 && !null_value ? 1 : 0;
}