bool Vers_parse_info::need_check(const Alter_info *alter_info) const
{
  return versioned_fields || unversioned_fields ||
         alter_info->flags & ALTER_ADD_PERIOD ||
         alter_info->flags & ALTER_DROP_PERIOD ||
         alter_info->flags & ALTER_ADD_SYSTEM_VERSIONING ||
         alter_info->flags & ALTER_DROP_SYSTEM_VERSIONING || *this;
}