bool Vers_parse_info::fix_implicit(THD *thd, Alter_info *alter_info)
{
  // If user specified some of these he must specify the others too. Do nothing.
  if (*this)
    return false;

  alter_info->flags|= ALTER_PARSER_ADD_COLUMN;

  period= start_end_t(default_start, default_end);
  as_row= period;

  if (vers_create_sys_field(thd, default_start, alter_info, VERS_ROW_START) ||
      vers_create_sys_field(thd, default_end, alter_info, VERS_ROW_END))
  {
    return true;
  }
  return false;
}