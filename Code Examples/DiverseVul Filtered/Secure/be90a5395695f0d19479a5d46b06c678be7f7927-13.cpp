MagickExport double *StringToArrayOfDoubles(const char *string,ssize_t *count,
  ExceptionInfo *exception)
{
  char
    *q;

  const char
    *p;

  double
    *array;

  register ssize_t
    i;

  /*
    Determine count of values, and check syntax.
  */
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  *count=0;
  if (string == (char *) NULL)
    return((double *) NULL);  /* no value found */
  i=0;
  p=string;
  while (*p != '\0')
  {
    (void) StringToDouble(p,&q);  /* get value - ignores leading space */
    if (p == q)
      return((double *) NULL);  /* no value found */
    p=q;
    i++;  /* increment value count */
    while (isspace((int) ((unsigned char) *p)) != 0)
      p++;  /* skip spaces */
    if (*p == ',')
      p++;  /* skip comma */
    while (isspace((int) ((unsigned char) *p)) != 0)
      p++;  /* and more spaces */
  }
  /*
    Allocate floating point argument list.
  */
  *count=i;
  array=(double *) AcquireQuantumMemory((size_t) i,sizeof(*array));
  if (array == (double *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'","");
      return((double *) NULL);
    }
  /*
    Fill in the floating point values.
  */
  i=0;
  p=string;
  while ((*p != '\0') && (i < *count))
  {
    array[i++]=StringToDouble(p,&q);
    p=q;
    while ((isspace((int) ((unsigned char) *p)) != 0) || (*p == ','))
      p++;
  }
  return(array);
}