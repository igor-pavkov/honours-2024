static char *ParseXPMColor(char *color,MagickBooleanType search_start)
{
#define NumberTargets  6

  char
    *p,
    *r;

  const char
    *q;

  ssize_t
    i;

  static const char
    *const targets[NumberTargets] = { "c ", "g ", "g4 ", "m ", "b ", "s " };

  if (*color == '\0')
    return((char *) NULL);
  if (search_start != MagickFalse)
    {
      for (i=0; i < NumberTargets; i++)
      {
        p=color;
        for (q=targets[i]; *p != '\0'; p++)
        {
          if (*p == '\n')
            break;
          if (*p != *q)
            continue;
          if (isspace((int) ((unsigned char) (*(p-1)))) == 0)
            continue;
          r=p;
          for ( ; ; )
          {
            if (*q == '\0')
              return(p);
            if (*r++ != *q++)
              break;
          }
          q=targets[i];
        }
      }
      return((char *) NULL);
    }
  for (p=color+1; *p != '\0'; p++)
  {
    if (*p == '\n')
      break;
    if (isspace((int) ((unsigned char) (*(p-1)))) == 0)
      continue;
    if (isspace((int) ((unsigned char) (*p))) != 0)
      continue;
    for (i=0; i < NumberTargets; i++)
    {
      if ((*p == *targets[i]) && (*(p+1) == *(targets[i]+1)))
        return(p);
    }
  }
  return(p);
}