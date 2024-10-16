int handler::update_first_row(const uchar *new_data)
{
  int error;
  if (likely(!(error= ha_rnd_init(1))))
  {
    int end_error;
    if (likely(!(error= ha_rnd_next(table->record[1]))))
    {
      /*
        We have to do the memcmp as otherwise we may get error 169 from InnoDB
      */
      if (memcmp(new_data, table->record[1], table->s->reclength))
        error= update_row(table->record[1], new_data);
    }
    end_error= ha_rnd_end();
    if (likely(!error))
      error= end_error;
    /* Logging would be wrong if update_row works but ha_rnd_end fails */
    DBUG_ASSERT(!end_error || error != 0);
  }
  return error;
}