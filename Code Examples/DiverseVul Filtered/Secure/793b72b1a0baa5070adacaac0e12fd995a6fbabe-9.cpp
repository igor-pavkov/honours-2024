int print_embedded_options(PerlIO *stream, char ** options_list, int options_count)
{
  int i;

  for (i=0; i<options_count; i++)
  {
    if (options_list[i])
        PerlIO_printf(stream,
                      "Embedded server, parameter[%d]=%s\n",
                      i, options_list[i]);
  }
  return 1;
}