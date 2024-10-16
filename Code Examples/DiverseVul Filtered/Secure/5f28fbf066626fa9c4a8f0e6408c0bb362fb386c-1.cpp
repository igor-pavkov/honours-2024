static char *sdb_find_arg(char *p)
{
  p++;
  while (*p==' ') p++;
  char *pp=p;
  while (*pp>' ') pp++;
  *pp='\0';
  return p;
}