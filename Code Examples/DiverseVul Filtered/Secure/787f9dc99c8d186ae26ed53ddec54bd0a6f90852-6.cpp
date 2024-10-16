static ssize_t sindex(int c,const char *string)
{
  register const char
    *p;

  for (p=string; *p != '\0'; p++)
    if (c == (int) (*p))
      return((ssize_t) (p-string));
  return(-1);
}