static unsigned int StringToTimeCode(const char *key)
{
  char
    buffer[2];

  register ssize_t
    i;

  unsigned int
    shift,
    value;

  value=0;
  shift=28;
  buffer[1]='\0';
  for (i=0; (*key != 0) && (i < 11); i++)
  {
    if (isxdigit((int) ((unsigned char) *key)) == 0)
      {
        key++;
        continue;
      }
    buffer[0]=(*key++);
    value|=(unsigned int) ((strtol(buffer,(char **) NULL,16)) << shift);
    shift-=4;
  }
  return(value);
}