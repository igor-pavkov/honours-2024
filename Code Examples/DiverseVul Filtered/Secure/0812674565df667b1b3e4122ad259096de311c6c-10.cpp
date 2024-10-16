static void formatString(Image *ofile, const char *s, int len)
{
  char
    temp[MaxTextExtent];

  (void) WriteBlobByte(ofile,'"');
  for (; len > 0; len--, s++) {
    int c = (*s) & 255;
    switch (c) {
    case '&':
      (void) WriteBlobString(ofile,"&amp;");
      break;
#ifdef HANDLE_GT_LT
    case '<':
      (void) WriteBlobString(ofile,"&lt;");
      break;
    case '>':
      (void) WriteBlobString(ofile,"&gt;");
      break;
#endif
    case '"':
      (void) WriteBlobString(ofile,"&quot;");
      break;
    default:
      if (isprint(c))
        (void) WriteBlobByte(ofile,(unsigned char) *s);
      else
        {
          (void) FormatLocaleString(temp,MaxTextExtent,"&#%d;", c & 255);
          (void) WriteBlobString(ofile,temp);
        }
      break;
    }
  }
#if defined(MAGICKCORE_WINDOWS_SUPPORT)
  (void) WriteBlobString(ofile,"\"\r\n");
#else
#if defined(macintosh)
  (void) WriteBlobString(ofile,"\"\r");
#else
  (void) WriteBlobString(ofile,"\"\n");
#endif
#endif
}