MagickExport char *StringInfoToHexString(const StringInfo *string_info)
{
  char
    *string;

  register const unsigned char
    *p;

  register ssize_t
    i;

  register unsigned char
    *q;

  size_t
    length;

  unsigned char
    hex_digits[16];

  length=string_info->length;
  if (~length < MagickPathExtent)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  string=(char *) AcquireQuantumMemory(length+MagickPathExtent,2*
    sizeof(*string));
  if (string == (char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  hex_digits[0]='0';
  hex_digits[1]='1';
  hex_digits[2]='2';
  hex_digits[3]='3';
  hex_digits[4]='4';
  hex_digits[5]='5';
  hex_digits[6]='6';
  hex_digits[7]='7';
  hex_digits[8]='8';
  hex_digits[9]='9';
  hex_digits[10]='a';
  hex_digits[11]='b';
  hex_digits[12]='c';
  hex_digits[13]='d';
  hex_digits[14]='e';
  hex_digits[15]='f';
  p=string_info->datum;
  q=(unsigned char *) string;
  for (i=0; i < (ssize_t) string_info->length; i++)
  {
    *q++=hex_digits[(*p >> 4) & 0x0f];
    *q++=hex_digits[*p & 0x0f];
    p++;
  }
  *q='\0';
  return(string);
}