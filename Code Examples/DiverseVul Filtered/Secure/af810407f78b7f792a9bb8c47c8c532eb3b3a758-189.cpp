bool Table_period_info::check_field(const Create_field* f,
                                    const Lex_ident& f_name) const
{
  bool res= false;
  if (!f)
  {
    my_error(ER_BAD_FIELD_ERROR, MYF(0), f_name.str, name.str);
    res= true;
  }
  else if (f->type_handler()->mysql_timestamp_type() != MYSQL_TIMESTAMP_DATE &&
           f->type_handler()->mysql_timestamp_type() != MYSQL_TIMESTAMP_DATETIME)
  {
    my_error(ER_WRONG_FIELD_SPEC, MYF(0), f->field_name.str);
    res= true;
  }
  else if (f->vcol_info || f->flags & VERS_SYSTEM_FIELD)
  {
    my_error(ER_PERIOD_FIELD_WRONG_ATTRIBUTES, MYF(0),
             f->field_name.str, "GENERATED ALWAYS AS");
    res= true;
  }

  return res;
}