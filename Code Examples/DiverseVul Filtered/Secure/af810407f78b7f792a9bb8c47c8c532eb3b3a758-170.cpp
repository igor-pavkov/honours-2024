bool Table_scope_and_contents_source_st::vers_fix_system_fields(
  THD *thd, Alter_info *alter_info, const TABLE_LIST &create_table)
{
  DBUG_ASSERT(!(alter_info->flags & ALTER_DROP_SYSTEM_VERSIONING));

  DBUG_EXECUTE_IF("sysvers_force", if (!tmp_table()) {
                  alter_info->flags|= ALTER_ADD_SYSTEM_VERSIONING;
                  options|= HA_VERSIONED_TABLE; });

  if (!vers_info.need_check(alter_info))
    return false;

  const bool add_versioning= alter_info->flags & ALTER_ADD_SYSTEM_VERSIONING;

  if (!vers_info.versioned_fields && vers_info.unversioned_fields && !add_versioning)
  {
    // All is correct but this table is not versioned.
    options&= ~HA_VERSIONED_TABLE;
    return false;
  }

  if (!add_versioning && vers_info && !vers_info.versioned_fields)
  {
    my_error(ER_MISSING, MYF(0), create_table.table_name.str,
             "WITH SYSTEM VERSIONING");
    return true;
  }

  List_iterator<Create_field> it(alter_info->create_list);
  while (Create_field *f= it++)
  {
    if (f->vers_sys_field())
      continue;
    if ((f->versioning == Column_definition::VERSIONING_NOT_SET && !add_versioning) ||
        f->versioning == Column_definition::WITHOUT_VERSIONING)
    {
      f->flags|= VERS_UPDATE_UNVERSIONED_FLAG;
    }
  } // while (Create_field *f= it++)

  if (vers_info.fix_implicit(thd, alter_info))
    return true;

  return false;
}