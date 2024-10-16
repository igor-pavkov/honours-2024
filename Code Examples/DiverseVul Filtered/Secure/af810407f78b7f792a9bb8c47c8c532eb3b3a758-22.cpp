void Alter_inplace_info::report_unsupported_error(const char *not_supported,
                                                  const char *try_instead) const
{
  if (unsupported_reason == NULL)
    my_error(ER_ALTER_OPERATION_NOT_SUPPORTED, MYF(0),
             not_supported, try_instead);
  else
    my_error(ER_ALTER_OPERATION_NOT_SUPPORTED_REASON, MYF(0),
             not_supported, unsupported_reason, try_instead);
}