static int str2num(long *val, char *str)
{
  int retcode = 0;
  if(str && ISDIGIT(*str))
    *val = atoi(str);
  else
    retcode = 1; /* badness */
  return retcode;
}