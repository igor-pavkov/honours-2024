bool Vers_parse_info::is_end(const Create_field &f) const
{
  return f.flags & VERS_ROW_END;
}