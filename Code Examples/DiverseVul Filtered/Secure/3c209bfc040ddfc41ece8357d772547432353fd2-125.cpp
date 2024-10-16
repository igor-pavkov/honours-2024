bool Item_singlerow_subselect::get_date(MYSQL_TIME *ltime,ulonglong fuzzydate)
{
  DBUG_ASSERT(fixed == 1);
  if (forced_const)
  {
    bool val= value->get_date(ltime, fuzzydate);
    null_value= value->null_value;
    return val;
  }
  if (!exec() && !value->null_value)
  {
    null_value= FALSE;
    return value->get_date(ltime, fuzzydate);
  }
  else
  {
    reset();
    DBUG_ASSERT(null_value);
    return 1;
  }
}