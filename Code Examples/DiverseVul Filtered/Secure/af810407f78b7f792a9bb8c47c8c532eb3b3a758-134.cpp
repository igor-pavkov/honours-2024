bool Vers_parse_info::is_end(const char *name) const
{
  DBUG_ASSERT(name);
  return as_row.end && as_row.end.streq(name);
}