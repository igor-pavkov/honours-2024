Event_queue_element::load_from_row(THD *thd, TABLE *table)
{
  char *ptr;
  MYSQL_TIME time;
  LEX_STRING tz_name;

  DBUG_ENTER("Event_queue_element::load_from_row");

  if (!table)
    DBUG_RETURN(TRUE);

  if (table->s->fields < ET_FIELD_COUNT)
    DBUG_RETURN(TRUE);

  if (load_string_fields(table->field,
                         ET_FIELD_DB, &dbname,
                         ET_FIELD_NAME, &name,
                         ET_FIELD_DEFINER, &definer,
                         ET_FIELD_TIME_ZONE, &tz_name,
                         ET_FIELD_COUNT))
    DBUG_RETURN(TRUE);

  if (load_time_zone(thd, tz_name))
    DBUG_RETURN(TRUE);

  starts_null= table->field[ET_FIELD_STARTS]->is_null();
  uint not_used;
  if (!starts_null)
  {
    table->field[ET_FIELD_STARTS]->get_date(&time, TIME_NO_ZERO_DATE);
    starts= my_tz_OFFSET0->TIME_to_gmt_sec(&time,&not_used);
  }

  ends_null= table->field[ET_FIELD_ENDS]->is_null();
  if (!ends_null)
  {
    table->field[ET_FIELD_ENDS]->get_date(&time, TIME_NO_ZERO_DATE);
    ends= my_tz_OFFSET0->TIME_to_gmt_sec(&time,&not_used);
  }

  if (!table->field[ET_FIELD_INTERVAL_EXPR]->is_null())
    expression= table->field[ET_FIELD_INTERVAL_EXPR]->val_int();
  else
    expression= 0;
  /*
    If neigher STARTS and ENDS is set, then both fields are empty.
    Hence, if ET_FIELD_EXECUTE_AT is empty there is an error.
  */
  execute_at_null= table->field[ET_FIELD_EXECUTE_AT]->is_null();
  DBUG_ASSERT(!(starts_null && ends_null && !expression && execute_at_null));
  if (!expression && !execute_at_null)
  {
    if (table->field[ET_FIELD_EXECUTE_AT]->get_date(&time,
                                                    TIME_NO_ZERO_DATE))
      DBUG_RETURN(TRUE);
    execute_at= my_tz_OFFSET0->TIME_to_gmt_sec(&time,&not_used);
  }

  /*
    We load the interval type from disk as string and then map it to
    an integer. This decouples the values of enum interval_type
    and values actually stored on disk. Therefore the type can be
    reordered without risking incompatibilities of data between versions.
  */
  if (!table->field[ET_FIELD_TRANSIENT_INTERVAL]->is_null())
  {
    int i;
    char buff[MAX_FIELD_WIDTH];
    String str(buff, sizeof(buff), &my_charset_bin);
    LEX_STRING tmp;

    table->field[ET_FIELD_TRANSIENT_INTERVAL]->val_str(&str);
    if (!(tmp.length= str.length()))
      DBUG_RETURN(TRUE);

    tmp.str= str.c_ptr_safe();

    i= find_string_in_array(interval_type_to_name, &tmp, system_charset_info);
    if (i < 0)
      DBUG_RETURN(TRUE);
    interval= (interval_type) i;
  }

  if (!table->field[ET_FIELD_LAST_EXECUTED]->is_null())
  {
    table->field[ET_FIELD_LAST_EXECUTED]->get_date(&time,
                                                   TIME_NO_ZERO_DATE);
    last_executed= my_tz_OFFSET0->TIME_to_gmt_sec(&time,&not_used);
  }

  if ((ptr= get_field(&mem_root, table->field[ET_FIELD_STATUS])) == NullS)
    DBUG_RETURN(TRUE);

  DBUG_PRINT("load_from_row", ("Event [%s] is [%s]", name.str, ptr));

  /* Set event status (ENABLED | SLAVESIDE_DISABLED | DISABLED) */
  switch (ptr[0])
  {
  case 'E' :
    status = Event_parse_data::ENABLED;
    break;
  case 'S' :
    status = Event_parse_data::SLAVESIDE_DISABLED;
    break;
  case 'D' :
  default:
    status = Event_parse_data::DISABLED;
    break;
  }
  if ((ptr= get_field(&mem_root, table->field[ET_FIELD_ORIGINATOR])) == NullS)
    DBUG_RETURN(TRUE);
  originator = (uint32) table->field[ET_FIELD_ORIGINATOR]->val_int(); 

  /* ToDo : Andrey . Find a way not to allocate ptr on event_mem_root */
  if ((ptr= get_field(&mem_root,
                      table->field[ET_FIELD_ON_COMPLETION])) == NullS)
    DBUG_RETURN(TRUE);

  on_completion= (ptr[0]=='D'? Event_parse_data::ON_COMPLETION_DROP:
                               Event_parse_data::ON_COMPLETION_PRESERVE);

  DBUG_RETURN(FALSE);
}