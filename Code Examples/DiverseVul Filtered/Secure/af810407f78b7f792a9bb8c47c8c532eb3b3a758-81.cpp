int ha_end()
{
  int error= 0;
  DBUG_ENTER("ha_end");

  /* 
    This should be eventually based on the graceful shutdown flag.
    So if flag is equal to HA_PANIC_CLOSE, the deallocate
    the errors.
  */
  if (unlikely(ha_finish_errors()))
    error= 1;

  DBUG_RETURN(error);
}