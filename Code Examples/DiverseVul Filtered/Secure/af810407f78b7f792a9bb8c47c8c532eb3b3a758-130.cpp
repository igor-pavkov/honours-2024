bool Vers_parse_info::check_sys_fields(const Lex_table_name &table_name,
                                       const Lex_table_name &db,
                                       Alter_info *alter_info,
                                       bool can_native) const
{
  if (check_conditions(table_name, db))
    return true;

  const Create_field *row_start= NULL;
  const Create_field *row_end= NULL;

  List_iterator<Create_field> it(alter_info->create_list);
  while (Create_field *f= it++)
  {
    if (!row_start && f->flags & VERS_ROW_START)
      row_start= f;
    else if (!row_end && f->flags & VERS_ROW_END)
      row_end= f;
  }

  if (!row_start || !row_end)
  {
    my_error(ER_VERS_PERIOD_COLUMNS, MYF(0), as_row.start.str, as_row.end.str);
    return true;
  }

  if (!can_native ||
      !row_start->is_some_bigint() ||
      !row_end->is_some_bigint())
  {
    if (row_start->vers_check_timestamp(table_name) ||
        row_end->vers_check_timestamp(table_name))
      return true;
  }
  else
  {
    if (row_start->vers_check_bigint(table_name) ||
        row_end->vers_check_bigint(table_name))
      return true;

    if (!TR_table::use_transaction_registry)
    {
      my_error(ER_VERS_TRT_IS_DISABLED, MYF(0));
      return true;
    }
  }

  return false;
}