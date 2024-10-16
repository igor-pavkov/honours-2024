bool Vers_parse_info::check_conditions(const Lex_table_name &table_name,
                                       const Lex_table_name &db) const
{
  if (!as_row.start || !as_row.end)
  {
    my_error(ER_MISSING, MYF(0), table_name.str,
                as_row.start ? "AS ROW END" : "AS ROW START");
    return true;
  }

  if (!period.start || !period.end)
  {
    my_error(ER_MISSING, MYF(0), table_name.str, "PERIOD FOR SYSTEM_TIME");
    return true;
  }

  if (!as_row.start.streq(period.start) ||
      !as_row.end.streq(period.end))
  {
    my_error(ER_VERS_PERIOD_COLUMNS, MYF(0), as_row.start.str, as_row.end.str);
    return true;
  }

  if (db.streq(MYSQL_SCHEMA_NAME))
  {
    my_error(ER_VERS_DB_NOT_SUPPORTED, MYF(0), MYSQL_SCHEMA_NAME.str);
    return true;
  }
  return false;
}