int handler::ha_rnd_init_with_error(bool scan)
{
  int error;
  if (likely(!(error= ha_rnd_init(scan))))
    return 0;
  table->file->print_error(error, MYF(0));
  return error;
}