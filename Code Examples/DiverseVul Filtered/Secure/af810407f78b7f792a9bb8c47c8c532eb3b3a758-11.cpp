static Create_field *vers_init_sys_field(THD *thd, const char *field_name, int flags, bool integer)
{
  Create_field *f= new (thd->mem_root) Create_field();
  if (!f)
    return NULL;

  f->field_name.str= field_name;
  f->field_name.length= strlen(field_name);
  f->charset= system_charset_info;
  f->flags= flags | NOT_NULL_FLAG;
  if (integer)
  {
    DBUG_ASSERT(0); // Not implemented yet
    f->set_handler(&type_handler_vers_trx_id);
    f->length= MY_INT64_NUM_DECIMAL_DIGITS - 1;
    f->flags|= UNSIGNED_FLAG;
  }
  else
  {
    f->set_handler(&type_handler_timestamp2);
    f->length= MAX_DATETIME_PRECISION;
  }
  f->invisible= DBUG_EVALUATE_IF("sysvers_show", VISIBLE, INVISIBLE_SYSTEM);

  if (f->check(thd))
    return NULL;

  return f;
}