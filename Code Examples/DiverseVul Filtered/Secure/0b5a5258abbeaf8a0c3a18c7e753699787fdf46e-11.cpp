Event_basic::load_time_zone(THD *thd, const LEX_STRING tz_name)
{
  String str(tz_name.str, &my_charset_latin1);
  time_zone= my_tz_find(thd, &str);

  return (time_zone == NULL);
}