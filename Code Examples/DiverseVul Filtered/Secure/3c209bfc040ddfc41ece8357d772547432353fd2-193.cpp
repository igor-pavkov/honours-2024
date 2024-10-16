String *Item_exists_subselect::val_str(String *str)
{
  DBUG_ASSERT(fixed == 1);
  if (!forced_const && exec())
    reset();
  str->set((ulonglong)value,&my_charset_bin);
  return str;
}