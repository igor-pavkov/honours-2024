static unsigned int PNMInteger(Image *image,const unsigned int base,
  ExceptionInfo *exception)
{
  int
    c;

  unsigned int
    value;

  /*
    Skip any leading whitespace.
  */
  do
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      return(0);
    if (c == (int) '#')
      c=PNMComment(image,exception);
  } while ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));
  if (base == 2)
    return((unsigned int) (c-(int) '0'));
  /*
    Evaluate number.
  */
  value=0;
  while (isdigit(c) != 0) {
    if (value > (unsigned int) (INT_MAX/10))
      break;
    value*=10;
    if (value > (unsigned int) (INT_MAX-(c-(int) '0')))
      break;
    value+=c-(int) '0';
    c=ReadBlobByte(image);
    if (c == EOF)
      return(0);
  }
  return(value);
}