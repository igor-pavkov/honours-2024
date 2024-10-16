MagickExport char **StringToArgv(const char *text,int *argc)
{
  char
    **argv;

  register const char
    *p,
    *q;

  register ssize_t
    i;

  *argc=0;
  if (text == (char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.
  */
  for (p=text; *p != '\0'; )
  {
    while (isspace((int) ((unsigned char) *p)) != 0)
      p++;
    if (*p == '\0')
      break;
    (*argc)++;
    if (*p == '"')
      for (p++; (*p != '"') && (*p != '\0'); p++) ;
    if (*p == '\'')
      for (p++; (*p != '\'') && (*p != '\0'); p++) ;
    while ((isspace((int) ((unsigned char) *p)) == 0) && (*p != '\0'))
      p++;
  }
  (*argc)++;
  argv=(char **) AcquireQuantumMemory((size_t) (*argc+1UL),sizeof(*argv));
  if (argv == (char **) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToConvertStringToARGV");
  /*
    Convert string to an ASCII list.
  */
  argv[0]=AcquireString("magick");
  p=text;
  for (i=1; i < (ssize_t) *argc; i++)
  {
    while (isspace((int) ((unsigned char) *p)) != 0)
      p++;
    q=p;
    if (*q == '"')
      {
        p++;
        for (q++; (*q != '"') && (*q != '\0'); q++) ;
      }
    else
      if (*q == '\'')
        {
          p++;
          for (q++; (*q != '\'') && (*q != '\0'); q++) ;
        }
      else
        while ((isspace((int) ((unsigned char) *q)) == 0) && (*q != '\0'))
          q++;
    argv[i]=(char *) AcquireQuantumMemory((size_t) (q-p)+MagickPathExtent,
      sizeof(**argv));
    if (argv[i] == (char *) NULL)
      {
        for (i--; i >= 0; i--)
          argv[i]=DestroyString(argv[i]);
        argv=(char **) RelinquishMagickMemory(argv);
        ThrowFatalException(ResourceLimitFatalError,
          "UnableToConvertStringToARGV");
      }
    (void) memcpy(argv[i],p,(size_t) (q-p));
    argv[i][q-p]='\0';
    p=q;
    while ((isspace((int) ((unsigned char) *p)) == 0) && (*p != '\0'))
      p++;
  }
  argv[i]=(char *) NULL;
  return(argv);
}