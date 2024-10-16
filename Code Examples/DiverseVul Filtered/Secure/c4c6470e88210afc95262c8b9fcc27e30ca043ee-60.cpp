const char* blosc_get_compressor(void)
{
  const char* compname;
  blosc_compcode_to_compname(g_compressor, &compname);

  return compname;
}