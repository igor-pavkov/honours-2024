static bool vers_create_sys_field(THD *thd, const char *field_name,
                                  Alter_info *alter_info, int flags)
{
  Create_field *f= vers_init_sys_field(thd, field_name, flags, false);
  if (!f)
    return true;

  alter_info->flags|= ALTER_PARSER_ADD_COLUMN;
  alter_info->create_list.push_back(f);

  return false;
}