bool Create_field::vers_check_bigint(const Lex_table_name &table_name) const
{
  if (is_some_bigint() && flags & UNSIGNED_FLAG &&
      length == MY_INT64_NUM_DECIMAL_DIGITS - 1)
    return false;

  my_error(ER_VERS_FIELD_WRONG_TYPE, MYF(0), field_name.str,
           "BIGINT(20) UNSIGNED", table_name.str);
  return true;
}