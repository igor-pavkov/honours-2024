static MagickBooleanType IsPoint(const char *point)
{
  char
    *p;

  ssize_t
    value;

  value=strtol(point,&p,10);
  (void) value;
  return(p != point ? MagickTrue : MagickFalse);
}