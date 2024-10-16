void blosc_set_delta(int dodelta) {

  g_delta = dodelta;

  /* Check whether the library should be initialized */
  if (!g_initlib) blosc_init();

}