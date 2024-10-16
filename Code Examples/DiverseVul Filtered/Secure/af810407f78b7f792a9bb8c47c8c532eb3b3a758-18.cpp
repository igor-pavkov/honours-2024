static int cmp_table_names_desc(LEX_CSTRING * const *a, LEX_CSTRING * const *b)
{
  return -cmp_table_names(a, b);
}