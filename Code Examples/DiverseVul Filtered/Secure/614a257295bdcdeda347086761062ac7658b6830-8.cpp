MagickExport char *ReadBlobString(Image *image,char *string)
{
  int
    c;

  register ssize_t
    i;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  for (i=0; i < (MaxTextExtent-1L); i++)
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      {
        if (i == 0)
          return((char *) NULL);
        break;
      }
    string[i]=c;
    if (c == '\n')
      {
        if ((i > 0) && (string[i-1] == '\r'))
          i--;
        break;
      }
  }
  string[i]='\0';
  return(string);
}