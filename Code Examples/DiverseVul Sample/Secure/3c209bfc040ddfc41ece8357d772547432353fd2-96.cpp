double Item_exists_subselect::val_real()
{
  DBUG_ASSERT(fixed == 1);
  if (!forced_const && exec())
  {
    reset();
    return 0;
  }
  return (double) value;
}