Vers_parse_info::fix_create_like(Alter_info &alter_info, HA_CREATE_INFO &create_info,
                                 TABLE_LIST &src_table, TABLE_LIST &table)
{
  List_iterator<Create_field> it(alter_info.create_list);
  List_iterator<Key> key_it(alter_info.key_list);
  List_iterator<Key_part_spec> kp_it;
  Create_field *f, *f_start=NULL, *f_end= NULL;

  DBUG_ASSERT(alter_info.create_list.elements > 2);

  if (create_info.tmp_table())
  {
    int remove= 2;
    while (remove && (f= it++))
    {
      if (f->flags & VERS_SYSTEM_FIELD)
      {
        it.remove();
        remove--;
      }
      key_it.rewind();
      while (Key *key= key_it++)
      {
        kp_it.init(key->columns);
        while (Key_part_spec *kp= kp_it++)
        {
          if (0 == lex_string_cmp(system_charset_info, &kp->field_name,
                                  &f->field_name))
          {
            kp_it.remove();
          }
        }
        if (0 == key->columns.elements)
        {
          key_it.remove();
        }
      }
    }
    DBUG_ASSERT(remove == 0);
    push_warning_printf(current_thd, Sql_condition::WARN_LEVEL_WARN,
                        ER_UNKNOWN_ERROR,
                        "System versioning is stripped from temporary `%s.%s`",
                        table.db.str, table.table_name.str);
    return false;
  }

  while ((f= it++))
  {
    if (f->flags & VERS_ROW_START)
    {
      f_start= f;
      if (f_end)
        break;
    }
    else if (f->flags & VERS_ROW_END)
    {
      f_end= f;
      if (f_start)
        break;
    }
  }

  if (!f_start || !f_end)
  {
    my_error(ER_MISSING, MYF(0), src_table.table_name.str,
             f_start ? "AS ROW END" : "AS ROW START");
    return true;
  }

  as_row= start_end_t(f_start->field_name, f_end->field_name);
  period= as_row;

  create_info.options|= HA_VERSIONED_TABLE;
  return false;
}