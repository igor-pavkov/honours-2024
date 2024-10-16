Table_scope_and_contents_source_st::fix_period_fields(THD *thd,
                                                      Alter_info *alter_info)
{
  if (!period_info.name)
    return false;

  Table_period_info::start_end_t &period= period_info.period;
  List_iterator<Create_field> it(alter_info->create_list);
  while (Create_field *f= it++)
  {
    if (period.start.streq(f->field_name) || period.end.streq(f->field_name))
    {
      f->period= &period_info;
      f->flags|= NOT_NULL_FLAG;
    }
  }
  return false;
}