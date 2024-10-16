static int cmp_table_names(LEX_CSTRING * const *a, LEX_CSTRING * const *b)
{
  return my_strnncoll(&my_charset_bin, (uchar*)((*a)->str), (*a)->length,
                                       (uchar*)((*b)->str), (*b)->length);
}