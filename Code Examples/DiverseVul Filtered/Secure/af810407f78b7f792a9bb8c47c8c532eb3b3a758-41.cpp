bool Vers_parse_info::fix_alter_info(THD *thd, Alter_info *alter_info,
                                     HA_CREATE_INFO *create_info, TABLE *table)
{
  TABLE_SHARE *share= table->s;
  const char *table_name= share->table_name.str;

  if (!need_check(alter_info) && !share->versioned)
    return false;

  if (DBUG_EVALUATE_IF("sysvers_force", 0, share->tmp_table))
  {
    my_error(ER_VERS_NOT_SUPPORTED, MYF(0), "CREATE TEMPORARY TABLE");
    return true;
  }

  if (alter_info->flags & ALTER_ADD_SYSTEM_VERSIONING &&
      table->versioned())
  {
    my_error(ER_VERS_ALREADY_VERSIONED, MYF(0), table_name);
    return true;
  }

  if (alter_info->flags & ALTER_DROP_SYSTEM_VERSIONING)
  {
    if (!share->versioned)
    {
      my_error(ER_VERS_NOT_VERSIONED, MYF(0), table_name);
      return true;
    }
#ifdef WITH_PARTITION_STORAGE_ENGINE
    if (table->part_info &&
        table->part_info->part_type == VERSIONING_PARTITION)
    {
      my_error(ER_DROP_VERSIONING_SYSTEM_TIME_PARTITION, MYF(0), table_name);
      return true;
    }
#endif

    return false;
  }

  if (!(alter_info->flags & ALTER_ADD_SYSTEM_VERSIONING))
  {
    List_iterator_fast<Create_field> it(alter_info->create_list);
    while (Create_field *f= it++)
    {
      if (f->flags & VERS_SYSTEM_FIELD)
      {
        if (!table->versioned())
        {
          my_error(ER_VERS_NOT_VERSIONED, MYF(0), table->s->table_name.str);
          return true;
        }
        my_error(ER_VERS_DUPLICATE_ROW_START_END, MYF(0),
                 f->flags & VERS_ROW_START ? "START" : "END", f->field_name.str);
        return true;
      }
    }
  }

  if ((alter_info->flags & ALTER_DROP_PERIOD ||
       versioned_fields || unversioned_fields) && !share->versioned)
  {
    my_error(ER_VERS_NOT_VERSIONED, MYF(0), table_name);
    return true;
  }

  if (share->versioned)
  {
    if (alter_info->flags & ALTER_ADD_PERIOD)
    {
      my_error(ER_VERS_ALREADY_VERSIONED, MYF(0), table_name);
      return true;
    }

    // copy info from existing table
    create_info->options|= HA_VERSIONED_TABLE;

    DBUG_ASSERT(share->vers_start_field());
    DBUG_ASSERT(share->vers_end_field());
    Lex_ident start(share->vers_start_field()->field_name);
    Lex_ident end(share->vers_end_field()->field_name);
    DBUG_ASSERT(start.str);
    DBUG_ASSERT(end.str);

    as_row= start_end_t(start, end);
    period= as_row;

    if (alter_info->create_list.elements)
    {
      List_iterator_fast<Create_field> it(alter_info->create_list);
      while (Create_field *f= it++)
      {
        if (f->versioning == Column_definition::WITHOUT_VERSIONING)
          f->flags|= VERS_UPDATE_UNVERSIONED_FLAG;

        if (f->change.str && (start.streq(f->change) || end.streq(f->change)))
        {
          my_error(ER_VERS_ALTER_SYSTEM_FIELD, MYF(0), f->change.str);
          return true;
        }
      }
    }

    return false;
  }

  return fix_implicit(thd, alter_info);
}