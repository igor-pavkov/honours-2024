int blosc_set_compressor(const char* compname) {
  int code = blosc_compname_to_compcode(compname);

  g_compressor = code;

  /* Check whether the library should be initialized */
  if (!g_initlib) blosc_init();

  return code;
}