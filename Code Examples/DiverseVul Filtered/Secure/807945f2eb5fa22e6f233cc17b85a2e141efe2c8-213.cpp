longlong Item_func_gt::val_int()
{
  DBUG_ASSERT(fixed == 1);
  int value= cmp.compare();
  return value > 0 ? 1 : 0;
}