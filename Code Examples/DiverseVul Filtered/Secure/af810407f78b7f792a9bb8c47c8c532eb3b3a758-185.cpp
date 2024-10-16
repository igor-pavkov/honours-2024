int handler::check_old_types()
{
  Field** field;

  if (!table->s->mysql_version)
  {
    /* check for bad DECIMAL field */
    for (field= table->field; (*field); field++)
    {
      if ((*field)->type() == MYSQL_TYPE_NEWDECIMAL)
      {
        return HA_ADMIN_NEEDS_ALTER;
      }
      if ((*field)->type() == MYSQL_TYPE_VAR_STRING)
      {
        return HA_ADMIN_NEEDS_ALTER;
      }
    }
  }
  return 0;
}