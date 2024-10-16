bool Table_scope_and_contents_source_st::vers_check_system_fields(
        THD *thd, Alter_info *alter_info, const Lex_table_name &table_name,
        const Lex_table_name &db, int select_count)
{
  if (!(options & HA_VERSIONED_TABLE))
    return false;

  uint versioned_fields= 0;

  if (!(alter_info->flags & ALTER_DROP_SYSTEM_VERSIONING))
  {
    uint fieldnr= 0;
    List_iterator<Create_field> field_it(alter_info->create_list);
    while (Create_field *f= field_it++)
    {
      /*
         The field from the CREATE part can be duplicated in the SELECT part of
         CREATE...SELECT. In that case double counts should be avoided.
         select_create::create_table_from_items just pushes the fields back into
         the create_list, without additional manipulations, so the fields from
         SELECT go last there.
       */
      bool is_dup= false;
      if (fieldnr >= alter_info->create_list.elements - select_count)
      {
        List_iterator<Create_field> dup_it(alter_info->create_list);
        for (Create_field *dup= dup_it++; !is_dup && dup != f; dup= dup_it++)
          is_dup= Lex_ident(dup->field_name).streq(f->field_name);
      }

      if (!(f->flags & VERS_UPDATE_UNVERSIONED_FLAG) && !is_dup)
        versioned_fields++;
      fieldnr++;
    }
    if (versioned_fields == VERSIONING_FIELDS)
    {
      my_error(ER_VERS_TABLE_MUST_HAVE_COLUMNS, MYF(0), table_name.str);
      return true;
    }
  }

  if (!(alter_info->flags & ALTER_ADD_SYSTEM_VERSIONING) && !versioned_fields)
    return false;

  bool can_native= ha_check_storage_engine_flag(db_type,
                                                HTON_NATIVE_SYS_VERSIONING)
                   || db_type->db_type == DB_TYPE_PARTITION_DB;

  return vers_info.check_sys_fields(table_name, db, alter_info, can_native);
}