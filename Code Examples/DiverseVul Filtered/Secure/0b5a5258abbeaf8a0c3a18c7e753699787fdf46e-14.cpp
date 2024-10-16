Event_basic::load_string_fields(Field **fields, ...)
{
  bool ret= FALSE;
  va_list args;
  enum enum_events_table_field field_name;
  LEX_STRING *field_value;

  DBUG_ENTER("Event_basic::load_string_fields");

  va_start(args, fields);
  field_name= (enum enum_events_table_field) va_arg(args, int);
  while (field_name < ET_FIELD_COUNT)
  {
    field_value= va_arg(args, LEX_STRING *);
    if ((field_value->str= get_field(&mem_root, fields[field_name])) == NullS)
    {
      ret= TRUE;
      break;
    }
    field_value->length= strlen(field_value->str);

    field_name= (enum enum_events_table_field) va_arg(args, int);
  }
  va_end(args);

  DBUG_RETURN(ret);
}