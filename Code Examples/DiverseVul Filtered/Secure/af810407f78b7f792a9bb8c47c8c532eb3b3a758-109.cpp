bool Vers_parse_info::is_start(const Create_field &f) const
{
  return f.flags & VERS_ROW_START;
}