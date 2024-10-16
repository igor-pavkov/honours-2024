bool Table_scope_and_contents_source_st::check_fields(
  THD *thd, Alter_info *alter_info,
  const Lex_table_name &table_name, const Lex_table_name &db, int select_count)
{
  return vers_check_system_fields(thd, alter_info,
                                  table_name, db, select_count) ||
    check_period_fields(thd, alter_info);
}