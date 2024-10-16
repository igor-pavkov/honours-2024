static ssize_t CopyXPMColor(char *destination,const char *source,size_t length)
{
  register const char
    *p;

  p=source;
  while (length-- && (*p != '\0'))
    *destination++=(*p++);
  if (length != 0)
    *destination='\0';
  return((ssize_t) (p-source));
}