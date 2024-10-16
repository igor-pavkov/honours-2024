char *xid_to_str(char *buf, const XID &xid)
{
  int i;
  char *s=buf;
  *s++='\'';
  for (i= 0; i < xid.gtrid_length + xid.bqual_length; i++)
  {
    uchar c= (uchar) xid.data[i];
    /* is_next_dig is set if next character is a number */
    bool is_next_dig= FALSE;
    if (i < XIDDATASIZE)
    {
      char ch= xid.data[i + 1];
      is_next_dig= (ch >= '0' && ch <='9');
    }
    if (i == xid.gtrid_length)
    {
      *s++='\'';
      if (xid.bqual_length)
      {
        *s++='.';
        *s++='\'';
      }
    }
    if (c < 32 || c > 126)
    {
      *s++='\\';
      /*
        If next character is a number, write current character with
        3 octal numbers to ensure that the next number is not seen
        as part of the octal number
      */
      if (c > 077 || is_next_dig)
        *s++=_dig_vec_lower[c >> 6];
      if (c > 007 || is_next_dig)
        *s++=_dig_vec_lower[(c >> 3) & 7];
      *s++=_dig_vec_lower[c & 7];
    }
    else
    {
      if (c == '\'' || c == '\\')
        *s++='\\';
      *s++=c;
    }
  }
  *s++='\'';
  *s=0;
  return buf;
}