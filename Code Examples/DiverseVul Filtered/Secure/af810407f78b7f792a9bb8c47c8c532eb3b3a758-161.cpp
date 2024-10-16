bool handler::get_error_message(int error, String* buf)
{
  DBUG_EXECUTE_IF("external_lock_failure",
                  buf->set_ascii(STRING_WITH_LEN("KABOOM!")););
  return FALSE;
}