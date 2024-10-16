static char *NextXPMLine(char *p)
{
  assert(p != (char *) NULL);
  p=strchr(p,'\n');
  if (p != (char *) NULL)
    p++;
  return(p);
}