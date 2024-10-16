static int cmp_file_names(const void *a, const void *b)
{
  CHARSET_INFO *cs= character_set_filesystem;
  char *aa= ((FILEINFO *)a)->name;
  char *bb= ((FILEINFO *)b)->name;
  return my_strnncoll(cs, (uchar*)aa, strlen(aa), (uchar*)bb, strlen(bb));
}