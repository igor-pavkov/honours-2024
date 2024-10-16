static void my_output_message(j_common_ptr cinfo)
{
  (*cinfo->err->format_message) (cinfo, errStr);
}