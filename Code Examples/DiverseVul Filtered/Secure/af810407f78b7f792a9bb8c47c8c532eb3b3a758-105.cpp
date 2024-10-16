int handler::read_first_row(uchar * buf, uint primary_key)
{
  int error;
  DBUG_ENTER("handler::read_first_row");

  /*
    If there is very few deleted rows in the table, find the first row by
    scanning the table.
    TODO remove the test for HA_READ_ORDER
  */
  if (stats.deleted < 10 || primary_key >= MAX_KEY ||
      !(index_flags(primary_key, 0, 0) & HA_READ_ORDER))
  {
    if (likely(!(error= ha_rnd_init(1))))
    {
      error= ha_rnd_next(buf);
      const int end_error= ha_rnd_end();
      if (likely(!error))
        error= end_error;
    }
  }
  else
  {
    /* Find the first row through the primary key */
    if (likely(!(error= ha_index_init(primary_key, 0))))
    {
      error= ha_index_first(buf);
      const int end_error= ha_index_end();
      if (likely(!error))
        error= end_error;
    }
  }
  DBUG_RETURN(error);
}