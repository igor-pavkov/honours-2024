bool Create_field::vers_check_timestamp(const Lex_table_name &table_name) const
{
  if (type_handler() == &type_handler_timestamp2 &&
      length == MAX_DATETIME_FULL_WIDTH)
    return false;

  my_error(ER_VERS_FIELD_WRONG_TYPE, MYF(0), field_name.str, "TIMESTAMP(6)",
           table_name.str);
  return true;
}