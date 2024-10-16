bool Item_exists_subselect::val_bool()
{
  DBUG_ASSERT(fixed == 1);
  if (!forced_const && exec())
  {
    reset();
    return 0;
  }
  return value != 0;
}