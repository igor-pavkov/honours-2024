Table_scope_and_contents_source_st::fix_create_fields(THD *thd,
                                                      Alter_info *alter_info,
                                                      const TABLE_LIST &create_table)
{
  return vers_fix_system_fields(thd, alter_info, create_table)
         || fix_period_fields(thd, alter_info);
}