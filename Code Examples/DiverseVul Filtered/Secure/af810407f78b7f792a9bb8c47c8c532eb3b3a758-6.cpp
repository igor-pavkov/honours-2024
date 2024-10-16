static int ha_finish_errors(void)
{
  /* Allocate a pointer array for the error message strings. */
  my_error_unregister(HA_ERR_FIRST, HA_ERR_LAST);
  my_free(handler_errmsgs);
  handler_errmsgs= 0;
  return 0;
}