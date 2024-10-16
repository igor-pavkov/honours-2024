GooString* HtmlOutputDev::mapEncodingToHtml(GooString* encoding)
{
  GooString* enc = encoding;
  for(int i = 0; HtmlEncodings[i][0] != nullptr; i++)
  {
    if( enc->cmp(HtmlEncodings[i][0]) == 0 )
    {
      delete enc;
      return new GooString(HtmlEncodings[i][1]);
    }
  }
  return enc; 
}