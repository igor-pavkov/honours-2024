bool Table_scope_and_contents_source_st::check_period_fields(
                THD *thd, Alter_info *alter_info)
{
  if (!period_info.name)
    return false;

  if (tmp_table())
  {
    my_error(ER_PERIOD_TEMPORARY_NOT_ALLOWED, MYF(0));
    return true;
  }

  Table_period_info::start_end_t &period= period_info.period;
  const Create_field *row_start= NULL;
  const Create_field *row_end= NULL;
  List_iterator<Create_field> it(alter_info->create_list);
  while (const Create_field *f= it++)
  {
    if (period.start.streq(f->field_name)) row_start= f;
    else if (period.end.streq(f->field_name)) row_end= f;

    if (period_info.name.streq(f->field_name))
    {
      my_error(ER_DUP_FIELDNAME, MYF(0), f->field_name.str);
      return true;
    }
  }

  bool res= period_info.check_field(row_start, period.start.str)
            || period_info.check_field(row_end, period.end.str);
  if (res)
    return true;

  if (row_start->type_handler() != row_end->type_handler()
      || row_start->length != row_end->length)
  {
    my_error(ER_PERIOD_TYPES_MISMATCH, MYF(0), period_info.name.str);
    res= true;
  }

  return res;
}