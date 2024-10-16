bool Vers_parse_info::is_start(const char *name) const
{
  DBUG_ASSERT(name);
  return as_row.start && as_row.start.streq(name);
}