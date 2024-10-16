longlong Item_exists_subselect::val_int()
{
  DBUG_ASSERT(fixed == 1);
  if (!forced_const && exec())
  {
    reset();
    return 0;
  }
  return value;
}